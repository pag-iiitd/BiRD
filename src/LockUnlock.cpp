/*trace the prev details: if done is found after any race remove that done and replay races */

#include "definitions.h"
#include "ThreadLocalData.h"
#include "Lock.h"
#include "MemoryAddr.h"
#include <mutex>
#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>
#include <map> 
#include <algorithm> 
#include <string>
#include <deque>
#include <semaphore.h>
#include <ctime>
#define window_size 2
//PINPLAY_ENGINE pinplay_engine;
//KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE,
//                    "pintool", "log", "0",
//                  "Activate the pinplay logger");

//KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE,
//                    "pintool", "replay", "0",
//                  "Activate the pinplay replayer");

// Contains knobs to filter out things to instrument
FILTER filter;
int start_s, stop_s;
int total = 0;
int total1 = 0;
bool continue_race = false;
bool not_wait = false;
string ins_l;
bool next_execute = false;
string ins_s;
int next_tid, next_count;
string split_str = "";
struct relax_element
{
    THREADID tid;
    char type;
    string ins;
    int i_count;
    ADDRINT addr;
    VectorClock* vc;
};
string dpor = "";
vector<pair<THREADID, vector<relax_element>>> relax_struct;
bool all = false;
ofstream endrun;
bool has_done = false;
bool wait_t1 = false;
bool post_t2 = false;
bool set_done = false;
bool last_waiting = false;
int race_point = 0;
int last_ins = 0;
bool only_race = false;
bool only_relax = false;
string detail_s = "";
string s = "";
int last0 = 0;
vector<bool> thread_fini;
bool stack_end = false;
bool finished = false;
int split = 0;
int thread_count;
bool done = true;
int tid1, tid2, count1, count2, relax_tid2, relax_count2;
string relax_ins2;
string target = "";
sem_t sem, dep, rev, wait_last;
string state1, state2;
vector<Lock*> allLocks;
list<MemoryAddr*> memSet;
bool first_run = false;
bool reverse_point = false;
int totalThreads = 0;
int totalins = 0;
PIN_LOCK GlobalLock;
TLS_KEY tls_key;
std::mutex mtx;
string bt_string = "";
set<ADDRINT> writeIntersection;
string event;
map<THREADID, THREADID> mapOfThreadIDs;
ofstream sharedAccesses;
ofstream races;
ofstream bt;
ofstream instructions;
ofstream runfrom;
ofstream details;
ofstream relax_detail;
ofstream relax_file;
string relax_s = "";
vector<string> prev_exec;
vector<string> done_vec;
vector<string> remove_done;
vector<string> explore_vec;
vector<string> execution;
struct sema
{
    sem_t s;
    int wait = 0;
};

deque<sema> semaphores;
vector<deque<int>> order;

struct state
{
    int tid;
    int count;
    bool invert = false;
    bool done = false;
};
state curr_state, next_state;
deque<state> stack;
bool executed = false;
bool waited = false;
template <class T>
inline void PRINT_ELEMENTS (const T& coll, ThreadLocalData *tld, const char* optcstr = "")
{
    typename T::const_iterator pos;

    tld->out << optcstr;
    for (pos = coll.begin(); pos != coll.end(); ++pos) {
        tld->out << *pos << ' ';
    }
    tld->out << std::endl;
}

template <class T>
inline void PRINT_ELEMENTS_OUTPUT (const T& coll, const char* optcstr = "")
{
    typename T::const_iterator pos;

    cout << optcstr;
    for (pos = coll.begin(); pos != coll.end(); ++pos) {
        cout << *pos << ' ';
    }
    cout << std::endl;
}

void updateMemoryClocks(ThreadLocalData* tld, Lock* lock) {
    set<ADDRINT>::const_iterator pos;

    for (pos = lock->memReadAccesses.begin(); pos != lock->memReadAccesses.end(); ++pos) {
        list<MemoryAddr*>::const_iterator lookup =
            find_if(memSet.begin(), memSet.end(), mem_has_addr(*pos));
        if (lookup != memSet.end()) {
            int j;
            int size = (*lookup)->accessingInstructions.size();
            for (j = 0; j < size; j++) {
                if (((*lookup)->accessingInstructions[j] > lock->lock_inst)
                        && ((*lookup)->accessingInstructions[j] < lock->unlock_inst)
                        && (tld->threadId == (*lookup)->accessingThread[j])) {
                    (*lookup)->accessClocks[j].receiveActionFromSpecialPoint(tld->currentVectorClock, tld->threadId);
                }
            }
        }

    }

    for (pos = lock->memWriteAccesses.begin(); pos != lock->memWriteAccesses.end(); ++pos) {
        list<MemoryAddr*>::const_iterator lookup =
            find_if(memSet.begin(), memSet.end(), mem_has_addr(*pos));
        if (lookup != memSet.end()) {
            int j;
            int size = (*lookup)->accessingInstructions.size();
            for (j = 0; j < size; j++) {
                if (((*lookup)->accessingInstructions[j] > lock->lock_inst)
                        && ((*lookup)->accessingInstructions[j] < lock->unlock_inst)
                        && (tld->threadId == (*lookup)->accessingThread[j])) {
                    (*lookup)->accessClocks[j].receiveActionFromSpecialPoint(tld->currentVectorClock, tld->threadId);
                }
            }
        }

    }

}

VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v) {
    cout << "Thread Start:" << threadid << endl;
    vector<relax_element> re;
    relax_element e;
    re.push_back(e);
    relax_struct.push_back(make_pair(threadid, re));
    ThreadLocalData* tld = new ThreadLocalData(threadid);

    if (threadid == 0) {
        //cout <<"here"<< endl;
        PIN_GetLock(&GlobalLock, tld->threadId);
        mapOfThreadIDs[threadid] = PIN_GetTid();
        PIN_ReleaseLock(&GlobalLock);

        stringstream fileName;
        fileName << "thread" << threadid << ".out";
        KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
                                    "o", fileName.str(), "specify output file name");
        tld->out.open(KnobOutputFile.Value().c_str());

        stringstream traceName;
        traceName << "thread_trace" << threadid << ".out";
        KNOB<string> KnobTraceFile(KNOB_MODE_WRITEONCE, "pintool",
                                   "o", traceName.str(), "specify output file name");
        tld->thread_trace.open(KnobTraceFile.Value().c_str());

        stringstream readSet;
        readSet << "readSet" << threadid << ".out";
        ifstream read(readSet.str().c_str());
        string line;
        for (unsigned int i = 1; getline(read, line); ++i)
            tld->readSet.insert(atoll(line.c_str()));

        stringstream writeSet;
        writeSet << "writeSet" << threadid << ".out";
        ifstream write(writeSet.str().c_str());
        for (unsigned int i = 1; getline(write, line); ++i)
            tld->writeSet.insert(atoll(line.c_str()));

        PIN_SetThreadData(tls_key, tld, threadid);
        PIN_GetLock(&GlobalLock, threadid);
        ++totalThreads;
        PIN_ReleaseLock(&GlobalLock);
    }
    else {
        THREADID parentThreadId = 0;
        PIN_GetLock(&GlobalLock, tld->threadId);
        mapOfThreadIDs[threadid] = PIN_GetTid();
        THREADID parentTid = PIN_GetParentTid();
        if (mapOfThreadIDs.count(parentTid) > 0)
            parentThreadId = mapOfThreadIDs[parentTid];
        PIN_ReleaseLock(&GlobalLock);

        ThreadLocalData* parentTls = getTLS(parentThreadId);

        PIN_GetLock(&parentTls->threadLock, threadid);

        parentTls->currentVectorClock->event();

        tld->currentVectorClock->receiveAction(parentTls->currentVectorClock);
        PIN_ReleaseLock(&parentTls->threadLock);

        stringstream fileName;
        fileName << "thread" << threadid << ".out";
        KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
                                    "o", fileName.str(), "specify output file name");
        tld->out.open(KnobOutputFile.Value().c_str());

        stringstream traceName;
        traceName << "thread_trace" << threadid << ".out";
        KNOB<string> KnobTraceFile(KNOB_MODE_WRITEONCE, "pintool",
                                   "o", traceName.str(), "specify output file name");
        tld->thread_trace.open(KnobTraceFile.Value().c_str());

        stringstream readSet;
        readSet << "readSet" << threadid << ".out";
        ifstream read(readSet.str().c_str());
        string line;
        for (unsigned int i = 1; getline(read, line); ++i)
            tld->readSet.insert(atoll(line.c_str()));

        stringstream writeSet;
        writeSet << "writeSet" << threadid << ".out";
        ifstream write(writeSet.str().c_str());
        for (unsigned int i = 1; getline(write, line); ++i)
            tld->writeSet.insert(atoll(line.c_str()));

        PIN_SetThreadData(tls_key, tld, threadid);
        PIN_GetLock(&GlobalLock, threadid);
        ++totalThreads;
        PIN_ReleaseLock(&GlobalLock);
    }
}

VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v) {
    all = true;


    ThreadLocalData* tld = getTLS(threadid);
    tld->out.close();
    //if (threadid == 0)
      //  last_ins = tld->insCount;
    if (threadid != 0) {

        THREADID parentThreadId = 0;
        PIN_GetLock(&GlobalLock, tld->threadId);
        mapOfThreadIDs[threadid] = PIN_GetTid();
        THREADID parentTid = PIN_GetParentTid();
        if (mapOfThreadIDs.count(parentTid) > 0)
            parentThreadId = mapOfThreadIDs[parentTid];
        PIN_ReleaseLock(&GlobalLock);

        ThreadLocalData *parentTls = getTLS(parentThreadId);

        parentTls->currentVectorClock->receiveAction(tld->currentVectorClock);
        parentTls->currentVectorClock->event();
    }
    cout << "Thread Finished:" << threadid << semaphores[0].wait <<semaphores[1].wait <<semaphores[2].wait<< endl;
    if (!first_run)
    {
        thread_fini[threadid] = true;
        {
           /* for (int i = 1; i < thread_count; i++)
            {
                if (!thread_fini[i])
                {
                    all = false;
                    break;
                }
            }
            if (all)
            {
                sem_post(&wait_last);
            }*/
        }
    }

    free(tld);
    PIN_SetThreadData(tls_key, 0, threadid);

}

VOID CheckCMPXCHG(THREADID threadid, INS ins) {
    ThreadLocalData* tld = getTLS(threadid);
    if (INS_Opcode(ins) == CMPXCHG) {
        tld->isCMPXCHG = 1;
    }
}

VOID CheckEAX(THREADID threadid, ADDRINT value, REG _reg, INS ins) {
    PIN_LockClient();
    ThreadLocalData* tld = getTLS(threadid);
    if (REG_FullRegName(_reg) == EAX_REG && value == 0) {
        tld->isEAXZero = 1;
    }
    if (REG_FullRegName(_reg) == EAX_REG && value == 2) {
        tld->isEAXTwo = 1;
    }
    PIN_UnlockClient();
}

VOID SetUpMemoryParams(THREADID threadid, ADDRINT effective_address, UINT32 size) {
    ThreadLocalData* tld = getTLS(threadid);
    MemoryWrite *memory = (MemoryWrite *) malloc(sizeof(MemoryWrite));
    memory->effective_address = effective_address;
    memory->size = size;
    tld->memoryOperandsBefore.push(memory);
}

VOID RecordMemoryWriteBeforeINS(THREADID threadid, INS ins) {
    PIN_LockClient();
    ThreadLocalData* tld = getTLS(threadid);
    ADDRINT value = 0;
    size_t res = 0;

    while (!tld->memoryOperandsBefore.empty()) {
        MemoryWrite *memoryWrite = tld->memoryOperandsBefore.front();
        tld->memoryOperandsBefore.pop();
        tld->memoryOperandsAfter.push(memoryWrite);

        res = PIN_SafeCopy(&value, (VOID *) memoryWrite->effective_address, memoryWrite->size);
        if (res == memoryWrite->size) {
            PIN_GetLock(&GlobalLock, tld->threadId);
            bool found;
            vector<Lock*>::const_iterator lookup =
                find_if(allLocks.begin(), allLocks.end(), lock_has_addr(memoryWrite->effective_address));
            if (lookup == allLocks.end())
                found = false;
            else
                found = (*lookup)->isLocked;
            PIN_ReleaseLock(&GlobalLock);

            if (value == 0 && !found) {
                tld->isZeroBefore = 1;
            }
            else if (value == 1 && !found) {
                tld->isOneBeforeAndUnlocked = 1;
            }

            else if (value == 1 && found) {
                tld->isOneBeforeAndLocked = 1;
            }
            else if (value == 2 && found)
                tld->isTwoBefore = 1;
        }
    }
    PIN_UnlockClient();
}

VOID RecordMemoryWriteAfterINS(THREADID threadid, INS ins) {
    PIN_LockClient();
    ThreadLocalData* tld = getTLS(threadid);
    ADDRINT value = 0;
    size_t res = 0;
    MemoryWrite *lockMemoryWrite = (MemoryWrite *) malloc(sizeof(MemoryWrite));

    while (!tld->memoryOperandsAfter.empty()) {
        MemoryWrite *memoryWrite = tld->memoryOperandsAfter.front();
        tld->memoryOperandsAfter.pop();

        res = PIN_SafeCopy(&value, (VOID *) memoryWrite->effective_address, memoryWrite->size);
        if (res == memoryWrite->size) {
            PIN_GetLock(&GlobalLock, tld->threadId);
            bool found;
            vector<Lock*>::const_iterator lookup =
                find_if(allLocks.begin(), allLocks.end(), lock_has_addr(memoryWrite->effective_address));
            if (lookup == allLocks.end())
                found = false;
            else
                found = (*lookup)->isLocked;
            PIN_ReleaseLock(&GlobalLock);

            if (value == 0 && found) {
                lockMemoryWrite = memoryWrite;
                tld->isZeroAfter = 1;
            }
            if (value == 1 && !found) {
                lockMemoryWrite = memoryWrite;
                tld->isOneAfterAndUnlocked = 1;
            }
            if (value == 1 && found) {
                lockMemoryWrite = memoryWrite;
                tld->isOneAfterAndLocked = 1;
            }
            if (value == 2 && !found) {
                lockMemoryWrite = memoryWrite;
                tld->isTwoAfter = 1;
            }
        }
    }

    int zeroOneLock = tld->isEAXZero && tld->isZeroBefore && tld->isOneAfterAndUnlocked;
    int zeroTwoLock = tld->isEAXTwo && tld->isZeroBefore && tld->isTwoAfter;

    if (zeroOneLock || zeroTwoLock) {
        PIN_GetLock(&GlobalLock, tld->threadId);
        tld->out << "Lock Detected" << endl;
        tld->out << INS_Disassemble(ins) << endl;
        tld->currentVectorClock->event();
        tld->out << lockMemoryWrite->effective_address << endl;

        set<ADDRINT>::iterator it = writeIntersection.find(lockMemoryWrite->effective_address);
        if (it != writeIntersection.end())
            writeIntersection.erase(it);
        vector<Lock*>::const_iterator lookup =
            find_if(allLocks.begin(), allLocks.end(), lock_has_addr(lockMemoryWrite->effective_address));
        if (lookup == allLocks.end()) {
            Lock *l = new Lock(lockMemoryWrite->effective_address);
            l->lock_it(tld->insCount);
            allLocks.push_back(l);
            l->lockVectorClock->receiveAction(tld->currentVectorClock);
            tld->acqLocks.push_back(l);
        }
        else {
            (*lookup)->lock_it(tld->insCount);
            (*lookup)->lockVectorClock->receiveAction(tld->currentVectorClock);
            tld->acqLocks.push_back((*lookup));
        }
        PIN_ReleaseLock(&GlobalLock);
    }

    int zeroOneUnlock = tld->isOneBeforeAndLocked && tld->isZeroAfter;
    int twoOneUnlock = tld->isTwoBefore && tld->isOneAfterAndLocked;
    int twoZeroUnlock = tld->isTwoBefore && tld->isZeroAfter;

    if (zeroOneUnlock || twoOneUnlock || twoZeroUnlock) {
        PIN_GetLock(&GlobalLock, tld->threadId);
        tld->out << "Unlocked" << endl;
        tld->currentVectorClock->event();
        tld->out << INS_Disassemble(ins) << endl;
        tld->out << lockMemoryWrite->effective_address << endl;

        vector<Lock*>::const_iterator lookup =
            find_if(allLocks.begin(), allLocks.end(), lock_has_addr(lockMemoryWrite->effective_address));
        if (lookup != allLocks.end()) {
            (*lookup)->unlock_it(tld->insCount);
            (*lookup)->lockVectorClock->receiveAction(tld->currentVectorClock);

            for (unsigned i = 0; i < (*lookup)->recordClocks.size(); ++i) {
                tld->out << "********************************" << endl;
                tld->out << "Locked Region " << i << endl;
                set<ADDRINT> readRegion = (*lookup)->atomicReadRegions[i];
                set<ADDRINT> writeRegion = (*lookup)->atomicWriteRegions[i];
                set<ADDRINT> out1;
                set<ADDRINT> out2;
                set<ADDRINT> out3;
                set_intersection(readRegion.begin(), readRegion.end(),
                                 (*lookup)->memWriteAccesses.begin(), (*lookup)->memWriteAccesses.end(),
                                 inserter(out1, out1.begin()));
                set_intersection(writeRegion.begin(), writeRegion.end(),
                                 (*lookup)->memReadAccesses.begin(), (*lookup)->memReadAccesses.end(),
                                 inserter(out2, out2.begin()));
                set_intersection(writeRegion.begin(), writeRegion.end(),
                                 (*lookup)->memWriteAccesses.begin(), (*lookup)->memWriteAccesses.end(),
                                 inserter(out3, out3.begin()));
                PRINT_ELEMENTS(out1, tld, "Rprev-Wcurr: ");
                PRINT_ELEMENTS(out2, tld, "Wprev-Rcurr: ");
                PRINT_ELEMENTS(out3, tld, "Wprev-Wcurr: ");
                tld->out << "***********************************" << endl;
                if ((!out1.empty()) || (!out2.empty()) || (!out3.empty())) {
                    tld->currentVectorClock->receiveAction(&((*lookup)->recordClocks[i]));
                }
            }

            (*lookup)->recordClocks.push_back(*((*lookup)->lockVectorClock));
            (*lookup)->atomicReadRegions.push_back((*lookup)->memReadAccesses);
            (*lookup)->atomicWriteRegions.push_back((*lookup)->memWriteAccesses);
            PRINT_ELEMENTS((*lookup)->memReadAccesses, tld, "Reads: ");
            PRINT_ELEMENTS((*lookup)->memWriteAccesses, tld, "Writes: ");

            updateMemoryClocks(tld, (*lookup));

            (*lookup)->memReadAccesses.clear();
            (*lookup)->memWriteAccesses.clear();
            list<Lock*>::iterator acqLock =
                find_if(tld->acqLocks.begin(), tld->acqLocks.end(), lock_has_addr((*lookup)->addr));
            if (acqLock != tld->acqLocks.end())
                tld->acqLocks.erase(acqLock);
        }
        PIN_ReleaseLock(&GlobalLock);
    }
    tld->resetVars();
    PIN_UnlockClient();
}

void check_lock(INS ins)
{
    UINT32 num_operands = INS_OperandCount(ins);
    UINT32 i;
    for (i = 0; i < num_operands; ++i) {
        if (INS_OperandWritten(ins, i)) {
            if (INS_OperandIsReg(ins, i)) {
                REG _reg = INS_OperandReg(ins, i);
                if (_reg != REG_INVALID() && _reg < REG_MM_BASE) {
                    INS_InsertCall(ins,
                                   IPOINT_BEFORE,
                                   (AFUNPTR) CheckCMPXCHG,
                                   IARG_THREAD_ID,
                                   IARG_PTR, ins,
                                   IARG_END);

                    INS_InsertCall(ins,
                                   IPOINT_BEFORE,
                                   (AFUNPTR) CheckEAX,
                                   IARG_THREAD_ID,
                                   IARG_REG_VALUE, _reg,
                                   IARG_PTR, _reg,
                                   IARG_PTR, ins,
                                   IARG_END);
                }
            }
            else if (INS_OperandIsMemory(ins, i)) {
                // Insert a call before to get the effective address and the size.
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR) SetUpMemoryParams,
                               IARG_THREAD_ID,
                               IARG_MEMORYWRITE_EA,
                               IARG_MEMORYWRITE_SIZE,
                               IARG_END);
                // Insert a call before to get the value before.
                INS_InsertCall(ins,
                               IPOINT_BEFORE,
                               (AFUNPTR) RecordMemoryWriteBeforeINS,
                               IARG_THREAD_ID,
                               IARG_PTR, ins,
                               IARG_END);
                // Insert a call after to get the value written.
                INS_InsertCall(ins,
                               IPOINT_AFTER,
                               (AFUNPTR) RecordMemoryWriteAfterINS,
                               IARG_THREAD_ID,
                               IARG_PTR, ins,
                               IARG_END);
            }
        }
    }
}

VOID incrementThreadINS(THREADID tid, INS ins) {
    ThreadLocalData *tld = getTLS(tid);
    tld->insCount++;
   instructions << tid << " " << tld->insCount << endl;
   
}
VOID MemoryReadInst(THREADID threadid, ADDRINT effective_address, int i )
{
    //cout << "Read Enter" << endl;
    relax_element read_element;
    list<MemoryAddr*>::const_iterator lookup =
        find_if(memSet.begin(), memSet.end(), mem_has_addr(effective_address));
    if (lookup != memSet.end()) {
        ThreadLocalData* tld = getTLS(threadid);
        cout << "Read Enter " <<threadid<<" "<<tld->insCount<<" "<<i<< endl;
        PIN_GetLock(&GlobalLock, tld->threadId);
        sharedAccesses << tld->threadId << " " << tld->insCount << " r " << effective_address << "," << endl;
        PIN_ReleaseLock(&GlobalLock);
        tld->addAddressToLockRead(effective_address);
        tld->currentVectorClock->event();
        PIN_GetLock(&((*lookup)->MemoryLock), tld->threadId);
        (*lookup)->operand_index.push_back(i);
        (*lookup)->accesses.push_back('r');
        (*lookup)->accessingThread.push_back(threadid);
        (*lookup)->accessingInstructions.push_back(tld->insCount);
        (*lookup)->accessClocks.push_back(*(tld->currentVectorClock));
	int size = (*lookup)->accesses.size();
	for (int k = 0; k < size-1; k++) {
           if ((*lookup)->accesses[k] == 'w') {
                    if ((*lookup)->accessClocks[k].areConcurrent(&((*lookup)->accessClocks[size-1]))) {
			instructions <<(*lookup)->accessingThread[k] <<" " << (*lookup)->accessingInstructions[k] <<" "<<(*lookup)->accessingThread[size-1] <<" " << (*lookup)->accessingInstructions[size-1] <<endl;
			}
		}
	}
		

        event = std::to_string(threadid) + "_" + std::to_string(tld->insCount) + "_" + "r_{" + std::to_string(threadid) + "}_{" + std::to_string(threadid) + "}_[]_{}";
        //bt_string=bt_string+event+" ";
        execution.push_back(event);
        read_element.tid = threadid;
        read_element.vc = tld->currentVectorClock;
        read_element.ins = ins_l;
        read_element.addr = effective_address;
        read_element.i_count = tld->insCount;
        read_element.type = 'r';

        for (std::vector<pair<THREADID, vector<relax_element>>>::iterator it = relax_struct.begin(); it != relax_struct.end(); ++it)
        {
            if (it->first == threadid)
            {
                it->second.push_back(read_element);
            }
        }
        PIN_ReleaseLock(&((*lookup)->MemoryLock));
    }
    //cout << "Read Exit" << endl;
}

VOID MemoryWriteInst(THREADID threadid, ADDRINT effective_address, int i) {

    relax_element write_element;
    list<MemoryAddr*>::const_iterator lookup =
        find_if(memSet.begin(), memSet.end(), mem_has_addr(effective_address));
    if (lookup != memSet.end()) {
        ThreadLocalData* tld = getTLS(threadid);
        cout << "Write Enter " <<threadid<<tld->insCount<< endl;
        PIN_GetLock(&GlobalLock, tld->threadId);
        sharedAccesses << tld->threadId << " " << tld->insCount << " w " << effective_address << "," << endl;
        PIN_ReleaseLock(&GlobalLock);
        tld->addAddressToLockWrite(effective_address);
        tld->currentVectorClock->event();
        PIN_GetLock(&((*lookup)->MemoryLock), tld->threadId);
        (*lookup)->accesses.push_back('w');
        (*lookup)->operand_index.push_back(i);
        (*lookup)->accessingThread.push_back(threadid);
        (*lookup)->accessingInstructions.push_back(tld->insCount);
        (*lookup)->accessClocks.push_back(*(tld->currentVectorClock));
	int size = (*lookup)->accesses.size();
	for (int k = 0; k < size-1; k++) {
             if ((*lookup)->accessClocks[k].areConcurrent(&((*lookup)->accessClocks[size-1]))) {
		 instructions <<(*lookup)->accessingThread[k] <<" " << (*lookup)->accessingInstructions[k] <<" "<<(*lookup)->accessingThread[size-1] <<" " <<	 (*lookup)->accessingInstructions[size-1] <<endl;
	 	}
	 }

        event = std::to_string(threadid) + "_" + std::to_string(tld->insCount) + "_" + "w_{" + std::to_string(threadid) + "}_{" + std::to_string(threadid) + "}_[]_{}";
        //bt_string=bt_string+event+" ";
        execution.push_back(event);
        write_element.tid = threadid;
        write_element.vc = tld->currentVectorClock;
        write_element.ins = ins_s;
        write_element.addr = effective_address;
        write_element.i_count = tld->insCount;
        write_element.type = 'w';
        for (std::vector<pair<THREADID, vector<relax_element>>>::iterator it = relax_struct.begin(); it != relax_struct.end(); ++it) {
            if (it->first == threadid)
            {
                it->second.push_back(write_element);
            }
        }
        PIN_ReleaseLock(&((*lookup)->MemoryLock));
    }
    //cout << "write exit" << endl;
}

void rec_mem(INS ins) {
    //cout << "Rec Enter" << endl;
    THREADID tid = PIN_ThreadId();
   // instructions << tid << " " << INS_Disassemble(ins) << endl;
    ThreadLocalData *tld = getTLS(tid);
    //cout << "rec "<<tid<<" "<<tld->insCount<<endl;

    INS_InsertCall(ins,
                   IPOINT_BEFORE,
                   (AFUNPTR) incrementThreadINS,
                   IARG_THREAD_ID,
                   IARG_PTR, ins,
                   IARG_END);

    UINT32 num_operands = INS_MemoryOperandCount(ins);
    UINT32 i;
    for (i = 0; i < num_operands; ++i) {
          
        if (INS_MemoryOperandIsRead(ins, i)) {
            ins_l = INS_Disassemble(ins);

            INS_InsertCall(ins,
                           IPOINT_BEFORE,
                           (AFUNPTR) MemoryReadInst,
                           IARG_THREAD_ID,
                           IARG_MEMORYOP_EA, i,
                           IARG_UINT32, i,
                           IARG_END);


        }


        if (INS_MemoryOperandIsWritten(ins, i)) {

            ins_s = INS_Disassemble(ins);
            INS_InsertCall(ins,
                           IPOINT_BEFORE,
                           (AFUNPTR) MemoryWriteInst,
                           IARG_THREAD_ID,
                           IARG_MEMORYOP_EA, i,
                           IARG_UINT32, i,
                           IARG_END);

        }
        //cout << "Rec Exit" << endl;

    }
}

VOID Trace(TRACE trace, VOID *val)/*(INS ins, VOID* v)*/
{
    string img_name = "";


    //if ((!IMG_IsMainExecutable(img)))
    //    return;
    //cout << "Trace Enter" << endl;

    PIN_GetLock(&GlobalLock, -1);
    ThreadLocalData *tld = getTLS(mapOfThreadIDs[PIN_GetTid()]);
    PIN_ReleaseLock(&GlobalLock);

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

            if (INS_IsAtomicUpdate(ins)) {

                check_lock(ins);
            }

            rec_mem(ins);


            tld->thread_trace << INS_Disassemble(ins) << endl;
            //cout << "Trace Exit" << endl;
        }

    }

}
VOID Fini(INT32 code, void *v)
{

for (int i = 0; i < execution.size(); i++)
    {
        cout << execution[i] <<endl;

    }


    string temp = "";

    list<MemoryAddr *>::const_iterator i;
    for (i = memSet.begin(); i != memSet.end(); i++) {
        cout << "**********************************" << endl;
        cout << (*i)->addr << endl;

        int size = (*i)->accesses.size();
        int k;
        for (k = 0; k < size; k++) {
            cout << k << ": " << endl;
            cout << (*i)->accessingThread[k] << endl;
            cout << (*i)->accessingInstructions[k] << endl;
            cout << (*i)->accesses[k] << endl;

            int j;
            for (j = 0; j < totalThreads; j++) {
                cout << (*i)->accessClocks[k].vclock_arr[j] << " ";
            }
            cout << endl;
        }
    }

    for (i = memSet.begin(); i != memSet.end(); i++) {
        int size = (*i)->accesses.size();
        int k;
        for (k = 0; k < size; k++) {
            int j;
            for (j = k + 1; j < size; j++) {
                if (j == k)
                    continue;
                if ((*i)->accesses[k] == 'w' || (*i)->accesses[j] == 'w') {
                    if ((*i)->accessClocks[k].areConcurrent(&((*i)->accessClocks[j]))) {
                        //  races << "**********************************" << endl;
                        //races << (*i)->addr << endl;
                        //races << "Race Detected" << endl;
                        races << (*i)->accessingThread[k] <<
                              " " << (*i)->accessingInstructions[k] <<
                              " " << (*i)->accesses[k] << " " /*<< (*i)->operand_index[k] << " " */ << (*i)->accessingThread[j] <<
                              " " << (*i)->accessingInstructions[j] <<
                              " " << (*i)->accesses[j] << /*" " << (*i)->operand_index[j] <<*/ endl;
                        s = std::to_string((*i)->accessingThread[k]) + "_" +  std::to_string((*i)->accessingInstructions[k]) + "_" + (*i)->accesses[k];
                        for ( int l = 0 ; l < execution.size(); l++ )
                        {
                            temp = execution[l];
                            size_t brk = temp.find_first_of('{');
                            string temp2 = temp.substr(0, brk - 1);

                            if ((std::find(done_vec.begin(), done_vec.end(), temp2) != done_vec.end()) && (std::find(remove_done.begin(), remove_done.end(), temp2) == remove_done.end()))
                            {
                                has_done = true;
                                string t1 = temp.substr(0, brk);
                                string t2 = temp.substr(brk);
                                temp = t1 + "<done>_" + t2;
                            }
                            if (((temp2.compare(split_str) == 0) && ((only_race) || (only_relax)) && (!set_done)) && (temp.find("<done>") == std::string::npos))
                            {
                                std::size_t brk = temp.find_first_of('{');
                                string t1 = temp.substr(0, brk);
                                string t2 = temp.substr(brk);
                                temp = t1 + "<done>_" + t2;
                                set_done = true;
                            }

                            std::size_t index = execution[l].find(s); /*if you find a race, search for instruction in execution*/
                            if ((index != std::string::npos) && (index < execution[l].find_first_of('{')))
                            {
                                total++;
                                if ((first_run) || (l <= split))
                                {
                                    if ((temp.at(temp.length() - 2) == '{') && (temp.at(temp.length() - 1) == '}'))
                                    {
                                        temp = temp.substr(0, temp.length() - 1);
                                        temp = temp + std::to_string((*i)->accessingThread[j]) + "_" +  std::to_string((*i)->accessingInstructions[j]) + "_" + (*i)->accesses[j] + "}";
                                    }
                                    else
                                    {
                                        temp = temp.substr(0, temp.length() - 1);
                                        temp = temp + "," + std::to_string((*i)->accessingThread[j]) + "_" +  std::to_string((*i)->accessingInstructions[j]) + "_" + (*i)->accesses[j] + "}";
                                    }
                                }

                                cout << l << " " << split << " " << only_race << " " << has_done << endl;
                                cout << temp2 << " " << has_done << endl;
                                if ((set_done) && (remove_done.size() != 0) && (l > split))
                                {
                                    if ((temp.at(temp.length() - 2) == '{') && (temp.at(temp.length() - 1) == '}'))
                                    {
                                        temp = temp.substr(0, temp.length() - 1);
                                        temp = temp + std::to_string((*i)->accessingThread[j]) + "_" +  std::to_string((*i)->accessingInstructions[j]) + "_" + (*i)->accesses[j] + "}";
                                    }
                                    else
                                    {
                                        temp = temp.substr(0, temp.length() - 1);
                                        temp = temp + "," + std::to_string((*i)->accessingThread[j]) + "_" +  std::to_string((*i)->accessingInstructions[j]) + "_" + (*i)->accesses[j] + "}";
                                    }

                                }
                            }
                            execution[l] = temp;
                        }
                    }
                }
            }
        }
    }

    for (int i = 0; i < totalThreads; i++)
    {

        for (int k = relax_struct[i].second.size() - 1; k > 0; k--)
        {

            for (int l = k - 1; (l >= k - window_size) && (l >= 0); l--)
            {
                // cout << "k l : "<<k<<" "<<l<< " " <<k-l<<endl;
                //cout << "address " << std::hex << relax_struct[i].second[l].addr << " " << std::hex << relax_struct[i].second[k].addr << endl;
                if ((relax_struct[i].second[l].type == 'w') && ((k - l) <= window_size) && (relax_struct[i].second[l].addr != relax_struct[i].second[k].addr)/*&&(relax_struct[i].second[l].tid==relax_struct[i].second[k].tid)*/)
                {
                    // cout << "address inside " << std::hex << relax_struct[i].second[l].addr << " " << std::hex << relax_struct[i].second[k].addr << endl;
                    
                    string local, sub1, sub2;
                    string prev_ins = std::to_string(relax_struct[i].second[l].tid) + "_" + std::to_string(relax_struct[i].second[l].i_count) + "_";
                    string next_ins = std::to_string(relax_struct[i].second[k].tid) + "_" + std::to_string(relax_struct[i].second[k].i_count) + "_" + relax_struct[i].second[k].ins;
                    for (int m = 0; m < execution.size(); m++)
                    {

                        std::size_t index = execution[m].find(prev_ins); /*if you find a race, search for instruction in execution*/
                        if ((index != std::string::npos) && (index < execution[l].find_first_of('{')))
                        {
                            local = execution[m];
                            std::size_t last_sq = local.find_last_of(']');
                            if (next_ins.find("mov") != std::string::npos)
                            {
				total1++;
                                if ((local.at(last_sq - 1) == '['))
                                {
                                    sub1 = local.substr(0, last_sq);
                                    sub2 = local.substr(last_sq);
                                    local = sub1 + next_ins + sub2;
                                }
                                else
                                {
                                    sub1 = local.substr(0, last_sq);
                                    sub2 = local.substr(last_sq);
                                    local = sub1 + "." + next_ins + sub2;
                                }
                                execution[m] = local;
                            }
                        }
                    }
                }
                if (relax_struct[i].second[l].addr == relax_struct[i].second[k].addr)
                    break;
            }
        }
    }

    /*Relax memory reordering detection goes here*/

    for (int i = 0; i < execution.size(); i++)
    {
        bt_string = bt_string + execution[i] + "\n";

    }

    /*if there was only one race to be relayed at race point move up else play the remaining races*/
    if (!first_run)
    {
        if ((!only_race) && (dpor == "race"))
        {
            runfrom.open("runfrom.out");
            runfrom << "detail" << endl;
            runfrom.close();
            for (int t = 1; t < prev_exec.size(); t++)
            {
                if (t != race_point)
                    detail_s = detail_s + prev_exec[t] + "\n";
                if (t == race_point)
                {
                    string tmp1 = prev_exec[t];
                    std::size_t last_c = tmp1.find_last_of(',');
                    tmp1 = tmp1.substr(0, last_c);
                    tmp1 = tmp1 + "}";
                    detail_s = detail_s + tmp1 + "\n";
                }
                char set_det[80] = "INFO=detail";
                putenv(set_det);
                /*store the same execution trace without the replayed race*/
            }
        }

        if ((only_race) && (dpor == "race"))
        {
            runfrom.open("runfrom.out");
            runfrom << "bt" << endl;
            runfrom.close();
            detail_s = bt_string;
            char set_var[80] = "RUN_INFO=backtrack";
            putenv(set_var);
            //system("echo $RUN_INFO");
        }
        if ((only_relax) && (dpor == "relax1"))
        {
            runfrom.open("runfrom.out");
            runfrom << "bt" << endl;
            runfrom.close();
            detail_s = bt_string;
            relax_file.open("relaxfile.out");
            relax_file  << relax_s << endl;
            relax_file.close();
        }
        if ((!only_relax) && (dpor == "relax1"))
        {
            runfrom.open("runfrom.out");
            runfrom << "bt" << endl;
            runfrom.close();
            detail_s = bt_string;
            relax_file.open("relaxfile.out");
            relax_file  << relax_s << endl;
            relax_file.close();
        }
        details.open("details.out");
        details << totalThreads << "\n" << detail_s << endl;
    }
    bt.open("backtrack.out");
    cout << "total ins : " << totalins << endl;
    bt_string = std::to_string(totalThreads) + "\n" + bt_string;
    if (dpor == "relax1")
    {
        //bt<<relax_s<<endl;
        if (first_run)
        {
            string last = "0_" + std::to_string(last_ins) + "_x_{0}_{0}_[]_{}";
            bt << relax_s /*<< last */ << endl;
        }
        else
            bt << relax_s << endl;

    }
    else
    {
        //bt << bt_string <<endl;
        if (first_run)
        {
            string last = "0_" + std::to_string(last_ins) + "_x_{0}_{0}_[]_{}";
            bt << bt_string /*<< last */ << endl;
        }
        else
            bt << bt_string  << /*prev_exec[prev_exec.size() - 1] << "\n" <<*/ endl;
    }
    bt.close();
    details.close();
    instructions.close();
    stop_s = clock();
    cout << "time: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
    cout << "total" << total1 << " " << total << endl;
}

INT32 Usage()
{
    cerr <<
         "This pin tool tries to find the locks and unlocks in a program.\n"
         "\n";

    cerr << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

void load_read_write_sets() {
    string line;
    ifstream writeins("writeSetIntersection.out");
    for (unsigned int i = 1; getline(writeins, line); ++i) {
        writeIntersection.insert(atoll(line.c_str()));
        MemoryAddr* mem = new MemoryAddr(atoll(line.c_str()));
        memSet.push_back(mem);
    }
}

int main(int argc, char * argv[])
{   state st;
    bool d = false;
    deque<int> dq;
    int p = 0;
    sem_init(&sem, 0, 0);
    sem_init(&dep, 0, 0);
    sem_init(&rev, 0, 0);
    sem_init(&wait_last, 0, 0);
    std::ifstream file0("runfrom.out");
    std::ifstream file1("backtrack.out");
    std::ifstream file2("details.out");
    instructions.open("instructions.out");
    FILE * pFile2;
    FILE * run_file;
    pFile2 = fopen ( "backtrack.out" , "r" );
    run_file = fopen ( "runfrom.out" , "r" );
    fseek(pFile2, 0, SEEK_END);
    fseek(run_file, 0, SEEK_END);
    if (ftell(pFile2) != 0)
    {   /*if backtrack is not empty, record the previous execution stack*/

        string sLine, info;
        if (ftell(run_file) != 0)
        {
            std::getline(file0, info);
            cout << info << "is info" << endl;
            if (info.compare("bt") == 0)
                d = false;
            if (info.compare("detail") == 0)
                d = true;
        }
        std::ifstream infile;
        if (d)
        {
            std::getline(file2, sLine);
            infile.open("details.out");
        }
        else
        {
            std::getline(file1, sLine);
            infile.open("backtrack.out");
        }


        string subs;
        while (std::getline(infile, subs))
        {
            if (subs != "")
            {
                prev_exec.push_back(subs);
                std::size_t f = subs.find_first_of('{');
                string x = subs.substr(0, f);
                std::size_t u = x.find_first_of('_');
                st.tid = std::stoi(x.substr(0, u));
                x = x.substr(u + 1);
                u = x.find_first_of('_');
                st.count = std::stoi(x.substr(0, u));
                cout << st.tid << " " << st.count << endl;
                if (p == 0) /*for the first run the order is initialized to 0*/
                {
                    for (int r = 0; r < st.tid; r++)
                    {
                        order.push_back(dq);
                    }
                }
                if (p > 0)
                {   /*for other runs, store into order*/
                    order[st.tid].push_back(st.count);
                }
                stack.push_back(st);
                p++;
            }
        }
    }
    /*   else{
      cout<<"Error: File has no more lines to read."<<endl;
      exit (EXIT_FAILURE);
      }*/

    else
    {
        first_run = true;
    }
    int i;
    if (!first_run)
    {
        for (i = prev_exec.size() - 1; i > 0; i--)
        {

            target = prev_exec[i];
            if (prev_exec[i].find("<done>") != std::string::npos)
            {
                std::size_t dn = target.find_first_of('<');
                //cout<<"there "<<target.substr(0,dn-1)<<endl;
                done_vec.push_back(target.substr(0, dn - 1));
            }
            if (prev_exec[i].find("<explore>") != std::string::npos)
            {
                std::size_t dn = target.find_first_of('<');
                explore_vec.push_back(target.substr(0, dn - 1));
            }
        }
        for (i = prev_exec.size() - 1; i > 0; i--)
        {
            target = prev_exec[i];
            std::size_t sb2 = target.find_last_of(']');
            std::size_t sb1 = target.find_first_of('[');
            std::size_t us1, us2;
            std::size_t open = target.find_last_of('{');
            std::size_t first = target.find_first_of('{');
            state1 = target.substr(0, first - 1);
            if (prev_exec[i].find("<done>") != std::string::npos)
            {
                std::size_t dn = target.find_first_of('<');
                cout << "there " << target.substr(0, dn - 1) << endl;
                remove_done.push_back(target.substr(0, dn - 1));
            }
            if (!((prev_exec[i].at(prev_exec[i].length() - 1) == '}') && (prev_exec[i].at(prev_exec[i].length() - 2) == '{')))
            {   /*find the reversible races from the bootom most state*/
                race_point = i; /*set up the index of bottom most race*/
                dpor = "race";

                std::size_t us = state1.find_first_of('_');
                tid1 = std::stoi(state1.substr(0, us));
                string temp1 = state1.substr(us + 1);
                std::size_t ls = temp1.find_first_of('_');
                count1 = std::stoi(temp1.substr(0, ls));
                string t = target.substr(open + 1);
                state2 = t;
                state2 = state2.substr(0, state2.length() - 1);
                cout << "state1 " << state2 << endl;
                if (state2.find(',') != std::string::npos)
                {
                    std::size_t comma = state2.find_last_of(',');
                    state2 = state2.substr(comma + 1);
                    us1 = state2.find_first_of('_');
                    tid2 = std::stoi(state2.substr(0, us1));
                    state2 = state2.substr(us1 + 1);
                    us2 = state2.find_first_of('_');
                    count2 = std::stoi(state2.substr(0, us2));
                    only_race = false;
                }
                else
                {
                    us1 = state2.find_first_of('_');
                    tid2 = std::stoi(state2.substr(0, us1));
                    state2 = state2.substr(us1 + 1);
                    us2 = state2.find_first_of('_');
                    count2 = std::stoi(state2.substr(0, us2));
                    only_race = true;
                }
                done = false;
                break;
            }
            if (prev_exec[i].at(sb2 - 1) != '[')
            {
                dpor = "relax1";
                race_point = i;
                state2 = target.substr(sb1 + 1, sb2 - sb1);
                if (state2.find('.') != std::string::npos)
                {
                    std::size_t dot = state2.find_last_of('.');
                    state2 = state2.substr(dot + 1);
                    us1 = state2.find_first_of('_');
                    relax_tid2 = std::stoi(state2.substr(0, us1));
                    state2 = state2.substr(us1 + 1);
                    us2 = state2.find_first_of('_');
                    relax_count2 = std::stoi(state2.substr(0, us2));
                    relax_ins2 = state2.substr(us2 + 1);
                    relax_ins2 = relax_ins2.substr(0, relax_ins2.length() - 1);
                    only_relax = false;
                }
                else
                {
                    us1 = state2.find_first_of('_');
                    relax_tid2 = std::stoi(state2.substr(0, us1));
                    state2 = state2.substr(us1 + 1);
                    us2 = state2.find_first_of('_');
                    relax_count2 = std::stoi(state2.substr(0, us2));
                    relax_ins2 = state2.substr(us2 + 1);
                    relax_ins2 = relax_ins2.substr(0, relax_ins2.length() - 1);
                    only_relax = true;
                }
                relax_detail.open("relaxdetail.out");
                relax_detail << state1 << " " << relax_tid2 << " " << relax_count2 << " $$$" << relax_ins2 << endl;
                relax_detail.close();
                done = false;
                break;
            }
        }
        if (i <= 0) 
        {
            finished = true;
            endrun.open("endrun.out");
            endrun << "true" << endl;
            endrun.close();
        }
        else if (dpor == "relax1")
        {
            finished = false;
            endrun.open("endrun.out");
            endrun << "relax1" << endl;
            endrun.close();
        }
        else if (dpor == "race")
        {
            finished = false;
            endrun.open("endrun.out");
            endrun << "race" << endl;
            endrun.close();
        }
    }
    if (dpor == "relax1")
    {   int t;
        for (t = 0; t < prev_exec.size(); t++)
        {
            if (t == i)
            {
                string p = prev_exec[i];
                size_t open = p.find_first_of('[');
                size_t close = p.find_last_of(']');
                string s1 = p.substr(0, open + 1);
                string s2 = p.substr(close);
                if (only_relax)
                {
                    relax_s = relax_s + s1 + s2 + "\n";
                }
                else
                {
                    size_t d = p.find_last_of('.');
                    string s3 = p.substr(0, d);
                    relax_s = relax_s + s3 + s2 + "\n";
                }
            }
            else
            {
                relax_s = relax_s + prev_exec[t] + "\n";
            }
        }
    }
    thread_count = stack.front().tid;
    for (int ii = 0; ii < thread_count; ii++)
    {
        thread_fini.push_back(false);
        sem_init(&semaphores[ii].s, 0, 0);
    }
    if (stack.size() > 2) {
        stack.pop_front();
        curr_state = stack.front();
        stack.pop_front();
        next_state = stack.front();
    }
    if ((dpor != "relax1") && (!first_run))
        continue_race = true;
    load_read_write_sets();
    sharedAccesses.open("sharedAccesses.out");
    races.open("races.out");
    allLocks.reserve(20);

    PIN_InitSymbols();
    if ( PIN_Init(argc, argv) )
    {
        return Usage();
    }

    cout << tid1 << " " << count1 << " " << tid2 << " " << count2 << endl;
    // pinplay_engine.Activate(argc, argv,
    // KnobPinPlayLogger, KnobPinPlayReplayer);
    start_s = clock();
    tls_key = PIN_CreateThreadDataKey(0);
    PIN_InitLock(&GlobalLock);

    PIN_AddThreadStartFunction(ThreadStart, 0);
    PIN_AddThreadFiniFunction(ThreadFini, 0);

    TRACE_AddInstrumentFunction(Trace, 0);

    PIN_AddFiniFunction(Fini, 0);

    filter.Activate();

    PIN_StartProgram();
    return 0;
}
