#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <set>
#include <cmath>
#include <map>
#include <vector>
#include <algorithm>
#include <sys/resource.h>

#include "pin.H"
#include "portability.H"
#include "instlib.H"
#include "pinplay.H"
using namespace std;

PINPLAY_ENGINE pinplay_engine;
KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE,
                             "pintool", "log", "0",
                             "Activate the pinplay logger");

KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE,
                               "pintool", "replay", "0",
                               "Activate the pinplay replayer");

/*
 * Name of the output file
 */

map<THREADID, set<ADDRINT> > readSets;
map<THREADID, set<ADDRINT> > writeSets;

/*
 * Thread specific data
 */
TLS_KEY tls_key;

INSTLIB::FILTER filter;

struct rlimit limit;

class ThreadLocalData
{
public:
    ThreadLocalData(THREADID tid);

    set<ADDRINT> readSet;
    set<ADDRINT> writeSet;
    PIN_LOCK threadlock;
    ADDRINT stackBase;
};

bool isStack(ThreadLocalData* tld, ADDRINT addr) {
    if ((addr > (tld->stackBase - limit.rlim_cur)) && (addr < (tld->stackBase)))
        return true;
    return false;
}

ThreadLocalData::ThreadLocalData(THREADID tid)
{
    PIN_InitLock(&threadlock);
}

ThreadLocalData* getTLS(THREADID tid)
{
    ThreadLocalData* tld = static_cast<ThreadLocalData*>(PIN_GetThreadData(tls_key, tid));
    return tld;
}


inline VOID  MemoryWriteInstrumentation(THREADID threadid, ADDRINT effectiveAddr) {
    ThreadLocalData * tld = getTLS(threadid);
    if (!isStack(tld, effectiveAddr))
        tld->writeSet.insert(effectiveAddr);
}


inline VOID  MemoryReadInstrumentation(THREADID threadid, ADDRINT effectiveAddr) {
    ThreadLocalData * tld = getTLS(threadid);
    if (!isStack(tld, effectiveAddr))
        tld->readSet.insert(effectiveAddr);

}


/*
 * Insert code to write data to a thread-specific buffer for instructions
 * that access memory.
 */
VOID Trace(TRACE trace, VOID *val)//(INS ins, VOID* v)
{
    string img_name = "";

    //if ((!IMG_IsMainExecutable(img)))
    //    return;

    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
        {
            SEC sec;
            IMG img;
            RTN rtn;
            rtn = INS_Rtn(ins);
            if (RTN_Valid(rtn))
            {
                sec = RTN_Sec(rtn);
                img = SEC_Img(sec);
            }
            else
                return;
            if (IMG_Valid (img))
                img_name = IMG_Name(img);
            if ((!filter.SelectTrace(trace)) && (img_name.find("libssl") == std::string::npos))
                return;
			    
            UINT32 memoryOperands = INS_MemoryOperandCount(ins);
            for (UINT32 memOp = 0; memOp < memoryOperands; memOp++)
            {
                // Note that if the operand is both read and written we log it once
                // for each.

                if (INS_MemoryOperandIsRead(ins, memOp))
                {

                    INS_InsertPredicatedCall(
                        ins, IPOINT_BEFORE, (AFUNPTR)MemoryReadInstrumentation,
                        IARG_THREAD_ID,
                        IARG_MEMORYOP_EA, memOp,
                        IARG_END);
                }

                if (INS_MemoryOperandIsWritten(ins, memOp))
                {

                    INS_InsertPredicatedCall(
                        ins, IPOINT_BEFORE, (AFUNPTR)MemoryWriteInstrumentation,
                        IARG_THREAD_ID,
                        IARG_MEMORYOP_EA, memOp,
                        IARG_END);
                }
            }
        }
    }
}


VOID ThreadStart(THREADID tid, CONTEXT *ctxt, INT32 flags, VOID *v)
{
    ThreadLocalData * tld = new ThreadLocalData(tid);
    tld->stackBase = PIN_GetContextReg(ctxt, REG_STACK_PTR);
    cout << "stackBase: " << tld->stackBase << endl;
    cout << "stackBase end: " << (tld->stackBase - limit.rlim_cur) << endl;
    PIN_SetThreadData(tls_key, tld, tid);
}


VOID ThreadFini(THREADID tid, const CONTEXT *ctxt, INT32 code, VOID *v)
{

    ThreadLocalData * tld = getTLS(tid);
    stringstream rs;
    rs << "readSet" << tid << ".out";
    string filename = rs.str();
    ofstream readSetFile;
    readSetFile.open(filename.c_str());

    stringstream ws;
    ws << "writeSet" << tid << ".out";
    filename = ws.str();
    ofstream writeSetFile;
    writeSetFile.open(filename.c_str());

    //store read set of a thread to a file.
    for (set<ADDRINT>::iterator i = tld->readSet.begin(); i != tld->readSet.end(); i++) {
        readSetFile << *i << endl;
    }

    //store write set of a thread to a file.
    for (set<ADDRINT>::iterator i = tld->writeSet.begin(); i != tld->writeSet.end(); i++) {
        writeSetFile << *i << endl;
    }

    readSets[tid] = tld->readSet;
    writeSets[tid] = tld->writeSet;

    PIN_SetThreadData(tls_key, 0, tid);
}

INT32 Usage()
{
    cerr << "This tool tries to use stack base to filter out shared memory addresses" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

VOID Fini(INT32 code, void *v) {
    set<ADDRINT> s1;
    set<ADDRINT> s2;

    set<ADDRINT> rsi;
    set<ADDRINT> wsi;

    ofstream readSetIntersection;
    readSetIntersection.open("readSetIntersection.out");

    ofstream writeSetIntersection;
    writeSetIntersection.open("writeSetIntersection.out");

    //Calculate pairwise intersection of writesets of all threads.
    for ( map<THREADID, set<ADDRINT> >::iterator ii = writeSets.begin(); ii != writeSets.end(); ++ii)
    {
        for ( map<THREADID, set<ADDRINT> >::iterator i2 = writeSets.begin(); i2 != writeSets.end(); ++i2) {
            if ((((*i2).first != (*ii).first)) || ((*i2).first == 0 && (*ii).first == 0)) {
                set<ADDRINT> out;
                s1 = (*i2).second;
                s2 = (*ii).second;
                set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(out, out.begin()));
                for (set<ADDRINT>::iterator i = out.begin(); i != out.end(); i++) {
                    wsi.insert(*i);
                }
            }
        }
    }

    //Calculate pairwise intersection of readsets of all threads.
    for ( map<THREADID, set<ADDRINT> >::iterator ii = readSets.begin(); ii != readSets.end(); ++ii)
    {
        for ( map<THREADID, set<ADDRINT> >::iterator i2 = readSets.begin(); i2 != readSets.end(); ++i2) {
            if ((((*i2).first != (*ii).first)) || ((*i2).first == 0 && (*ii).first == 0)) {
                set<ADDRINT> out;
                s1 = (*i2).second;
                s2 = (*ii).second;
                set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(out, out.begin()));
                for (set<ADDRINT>::iterator i = out.begin(); i != out.end(); i++) {
                    rsi.insert(*i);

                }
            }
        }
    }

    //write intersection of readsets to file.
    for (set<ADDRINT>::iterator i = rsi.begin(); i != rsi.end(); i++) {
        readSetIntersection << *i << endl;
    }

    //write intersections of writesets to file.
    for (set<ADDRINT>::iterator i = wsi.begin(); i != wsi.end(); i++) {
        writeSetIntersection << *i << endl;
    }
}


/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */
int main(int argc, char *argv[])
{
    // Initialize PIN library. Print help message if -h(elp) is specified
    // in the command line or the command line is invalid
    if ( PIN_Init(argc, argv) )
    {
        return Usage();
    }

    getrlimit(RLIMIT_STACK, &limit);

    pinplay_engine.Activate(argc, argv,
                            KnobPinPlayLogger, KnobPinPlayReplayer);
    // Initialize thread-specific data not handled by buffering api.
    tls_key = PIN_CreateThreadDataKey(0);

    // add an instrumentation function
    TRACE_AddInstrumentFunction(Trace, 0);

    // add callbacks
    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);

    PIN_AddFiniFunction(Fini, 0);

    filter.Activate();
    // Start the program, never returns
    PIN_StartProgram();

    return 0;
}
