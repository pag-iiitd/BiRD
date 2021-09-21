	/*trace the prev details: if done is found after any race remove that done and replay races */
	#include "Control.h"
	#include "definitions.h"
	#include "ThreadLocalData.h"
	#include "Lock.h"
	#include "MemoryAddr.h"
	#include <mutex>
	#include <cstdlib>
	#include <unistd.h>
	#include <thread>
	#include <stdlib.h>
	#include <algorithm> 
	#include <string>
	#include <map>
	#include <deque>
	#include <semaphore.h>
	#include <ctime>
	#include <initializer_list>

	#define window_size 2
	#define PTHREAD_CREATE "pthread_create"
	#define PTHREAD_JOIN "pthread_join"
	#define PTHREAD_MUTEX_LOCK "pthread_mutex_lock"
	#define PTHREAD_MUTEX_UNLOCK "pthread_mutex_unlock"

	//PINPLAY_ENGINE pinplay_engine;
	//KNOB<BOOL> KnobPinPlayLogger(KNOB_MODE_WRITEONCE,
	//                    "pintool", "log", "0",
	//                  "Activate the pinplay logger");

	//KNOB<BOOL> KnobPinPlayReplayer(KNOB_MODE_WRITEONCE,
	//                    "pintool", "replay", "0",
	//                  "Activate the pinplay replayer");

	// Contains knobs to filter out things to instrument

std::list<ADDRINT> addresses;
PIN_LOCK l;
int lable=0;
deque<writeRelax> writeRelaxQueue;
FILTER filter;
int bblStart = 0;
bool flushAll = false;
int bblSize = 0;
int lastIns = 0;
deque<threadInfo> threadInfoMap;
bool in_ea = false;
relax_element state1;
int diff = 0;
deque<lockInfo> lockAddr;
deque <startInfo> startInfoMap;
ofstream instructions;
deque<state> exec_after;
deque <fence_element> fence_map;
bool del = false;
deque<deque<relax_element>> curr_lock;
deque<bt_state> bt_table;
deque<bt_state> bt_prev;
int threadStarted[100];
int threadFinished[100];
deque<relax_element> racepoint_sleep;
deque<sleep_element> race_sleep;
int start_s, stop_s;
CONTEXT *ctxtx = new CONTEXT;
state break_point;
bool done = false;
bool released = false;
bool endrun_set = false;
int stack_size;
bool second_done = false;
bool wait_at_break[2] = {false, false};
bool reached_breakpoint = false;
int total = 0;
int total1 = 0;
string second;
bool pre_executed = false;
bool not_wait = false;
string ins_l;
bool next_execute = false;
string ins_s;
int next_tid, next_count;
deque<relax_info> relax_ds;
deque <pair<relax_element, relax_element>> fork_pair, join_pair;
deque <pair<ADDRINT,deque <pair<relax_element, relax_element>>>> lock_addr_pair;
vector<pair<THREADID, vector<relax_element>>> relax_struct;
ofstream endrun;
bool race = true;
int cnt = 0;
bool wait_t1 = false;
bool post_t2 = false;
int race_point = -1;
bool stack_end = false;
int thread_count;
int tid1, tid2, count1, count2;
vector<Lock*> allLocks;
list<MemoryAddr*> memSet;
bool first_run = false;
int totalThreads = 0;
PIN_LOCK GlobalLock;
TLS_KEY tls_key;
set<ADDRINT> writeIntersection;
string event;
map<THREADID, THREADID> mapOfThreadIDs;
	//ofstream sharedAccesses;
ofstream races;
ofstream relax_detail;
ofstream bt;
struct sema
{
	sem_t s;
	int wait = 0;
};


	//deque<sema> semaphores;
sema semaphores[100];
vector<deque<stack_element>> order;


state curr_state, next_state;
deque<state> stack;
deque<state> deleted_state;
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

	cout << "PIN: " << optcstr;
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

bool causallyPrecedes(relax_element event1, relax_element event2)
{
	cout << "In Causally Precedes" << endl;
	if (event1.tid == event2.tid)
		return true;
	for (std::deque <threadInfo>::iterator ti = threadInfoMap.begin(); ti != threadInfoMap.end(); ++ti)
	{
		if (ti->tid == event1.tid)
		{
			for (std::deque <threadInfo>::iterator tim = threadInfoMap.begin(); tim != threadInfoMap.end(); ++tim)
			{
				if (tim->tid == event2.tid)
				{
					if (tim->start > ti->end || ti->start > tim->end)
						return true;
				}
			}
		}
	}
	for (std::deque <pair<relax_element, relax_element>>::iterator fp = fork_pair.begin(); fp != fork_pair.end(); ++fp)
	{
		if (fp->first.tid == event1.tid && fp->second.tid ==event2.tid)
		{
			if ((event1.i_count < fp->first.i_count) && (event2.i_count > fp->second.i_count))
				return true;
		}
		if (fp->first.tid == event2.tid && fp->second.tid ==event1.tid)
		{
			if ((event2.i_count < fp->first.i_count) && (event1.i_count > fp->second.i_count))
				return true;
		}
	}
	for (std::deque <pair<relax_element, relax_element>>::iterator jp = join_pair.begin(); jp != join_pair.end(); ++jp)
	{
		if (jp->first.tid == event1.tid && jp->second.tid ==event2.tid)
		{
			if ((event1.i_count <= jp->first.i_count) && (event2.i_count >= jp->second.i_count))
				return true;
		}
		if (jp->first.tid == event2.tid && jp->second.tid ==event1.tid)
		{
			if ((event2.i_count <= jp->first.i_count) && (event1.i_count >= jp->second.i_count))
				return true;
		}
	}
	for (std::deque<pair<ADDRINT,deque <pair<relax_element, relax_element>>>>::iterator lap = lock_addr_pair.begin();lap != lock_addr_pair.end(); ++lap)
	{
		for (std::deque <pair<relax_element, relax_element>>::iterator lp = lap->second.begin(); lp != lap->second.end(); ++lp)
		{
			for (std::deque <pair<relax_element, relax_element>>::iterator up = lap->second.begin(); up != lap->second.end(); ++up)
			{
				if(lp != up)
				{
					if((event1.tid == lp->first.tid && event2.tid == up->first.tid) && (event1.i_count <= lp->second.i_count && event2.i_count > up->first.i_count))
					{
						if (event2.i_count <= up->second.i_count && event1.i_count > lp->first.i_count)
						{
							return true;
							break;
						}
					}
					if((event2.tid == lp->first.tid && event1.tid == up->first.tid) && (event1.i_count <= lp->second.i_count && event1.i_count > up->first.i_count))
					{
						if (event1.i_count <= up->second.i_count && event2.i_count > lp->first.i_count)
						{
							return true;
							break;
						}
					}
				}
			}
		}
	}
	for (std::deque<lockInfo>::iterator ad = lockAddr.begin(); ad != lockAddr.end(); ++ad)
	{
		for (std::deque<lockedRegion>::iterator lr1 = ad->locked_region.begin(); lr1 != ad->locked_region.end(); ++lr1)
		{
			for (std::deque<lockedRegion>::iterator lr2 = lr1 + 1 ; lr2 != ad->locked_region.end(); ++lr2)
			{
				if (lr1->tid != lr2->tid)
				{
					if ((lr1->tid == event1.tid) && (lr1->start <= event1.i_count) && (lr1->end >= event1.i_count))
					{
						if ((lr2->tid == event2.tid) && (lr2->start <= event2.i_count) && (lr2->end >= event2.i_count))
						{
							return true;
						}
					}
					if ((lr1->tid == event2.tid) && (lr1->start <= event2.i_count) && (lr1->end >= event2.i_count))
					{
						if ((lr2->tid == event1.tid) && (lr2->start <= event1.i_count) && (lr2->end >= event1.i_count))
						{
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}


VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v) {
	threadFinished[threadid] = 0;
	std::cout << std::hex << std::internal << std::setfill('0') 
	    << "Thread start RAX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RAX) << " " 
	    << "RBX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBX) << " " 
	    << "RCX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RCX) << std::endl
	    << "RDX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RDX) << " " 
	    << "RDI = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RDI) << " " 
	    << "RSI = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RSI) << std::endl
	    << "RBP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBP) << " "
	    << "RSP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RSP) << " "
	    << "RIP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RIP) << std::endl;
	std::cout << "+-------------------------------------------------------------------" << std::endl;

	   //cout << "Thread start Context: " << ctxt << " " << &threadid<< endl;
	cout << "PIN: Thread Start:" << threadid <<  endl;
	   //cout << "PIN: Thread Start Details:" << threadid  << " " << PIN_GetTid() << " " << PIN_ThreadId() <<" " << PIN_ThreadUid() <<  endl;
	vector<relax_element> re;
	deque<relax_element> le;
	relax_element e;
	re.push_back(e);
	sema sema_t;
	threadStarted[threadid] = 1;
	while (curr_lock.size() <= threadid)
	{
		curr_lock.push_back(le);
	}
	    //semaphores.push_back(sema_t);
	relax_struct.push_back(make_pair(threadid, re));
	ThreadLocalData* tld = new ThreadLocalData(threadid);
	sem_init(&semaphores[threadid].s, 0, 0);
	if (threadid == 0) {
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
		relax_element parent, child;
		THREADID parentThreadId = 0;
		PIN_GetLock(&GlobalLock, tld->threadId);
		mapOfThreadIDs[threadid] = PIN_GetTid();
		THREADID parentTid = PIN_GetParentTid();
		if (mapOfThreadIDs.count(parentTid) > 0)
			parentThreadId = mapOfThreadIDs[parentTid];
		PIN_ReleaseLock(&GlobalLock);

		ThreadLocalData* parentTls = getTLS(parentThreadId);

		PIN_GetLock(&parentTls->threadLock, threadid);

	        //parentTls->currentVectorClock->event();

		tld->currentVectorClock->receiveAction(parentTls->currentVectorClock);
		tld->currentVectorClock->event();

		parent.tid = parentThreadId;
		parent.i_count = parentTls->insCount;
		child.tid = threadid;
		child.i_count = tld->insCount;
		fork_pair.push_back(make_pair(parent, child));
		cout << "fork pair: " << child.tid << " " << child.i_count << " " << parent.tid << " " << parent.i_count << endl;
		
		threadInfo ti;
		ti.parent = parentThreadId;
		ti.tid = threadid;
		ti.regAddr = PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBX);
		for (std::deque<startInfo>::iterator si_iter = startInfoMap.begin(); si_iter != startInfoMap.end(); ++si_iter)
		{
			if(si_iter->tid == threadid)
			{
				ti.init_addr = si_iter->start_addr;
				ti.start = si_iter->start_count;
			}
		}
		threadInfoMap.push_back(ti);
		
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
bool allThreadFini()
{
	for (int i = 1; i < thread_count; i++)
	{
		if (threadFinished[i] == 0)
			return false;
	}
	return true;
}
VOID ThreadFini(THREADID threadid, const CONTEXT *ctxt, INT32 code, VOID *v) {
	cout << "IN Fini " << threadid << endl;

	std::cout << std::hex << std::internal << std::setfill('0') 
	    << "RAX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RAX) << " " 
	    << "RBX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBX) << " " 
	    << "RCX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RCX) << std::endl
	    << "RDX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RDX) << " " 
	    << "RDI = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RDI) << " " 
	    << "RSI = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RSI) << std::endl
	    << "RBP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBP) << " "
	    << "RSP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RSP) << " "
	    << "RIP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RIP) << std::endl;
	std::cout << "+-------------------------------------------------------------------" << std::endl;
	    //cout << "Thread finish Context: " << ctxt << " " << &ctxt  <<" " << &threadid<< endl;
	    //cout << "PIN: Thread Finish Details:" << threadid  << " " << PIN_GetTid() << " " << PIN_ThreadId() <<" " << PIN_ThreadUid() <<  endl;
	ThreadLocalData* tld = getTLS(threadid);
	if (threadid == 0)
	{
		lastIns = tld->insCount;
	}
	relax_element child, parent;
	threadStarted[threadid] = 0;
	threadFinished[threadid] = 1;

	if (threadid != 0) {
		if (semaphores[0].wait > 0)
		{
			semaphores[0].wait--;
			sem_post(&semaphores[0].s);
		}
		THREADID parentThreadId = 0;
		PIN_GetLock(&GlobalLock, tld->threadId);
		mapOfThreadIDs[threadid] = PIN_GetTid();
		THREADID parentTid = PIN_GetParentTid();
		if (mapOfThreadIDs.count(parentTid) > 0)
			parentThreadId = mapOfThreadIDs[parentTid];

		PIN_ReleaseLock(&GlobalLock);

		ThreadLocalData *parentTls = getTLS(parentThreadId);
		child.tid = threadid;
		child.i_count = tld->insCount;
		parent.tid = parentThreadId;
		if (threadFinished[0] != 1)
			parent.i_count = parentTls->insCount;
		else
			parent.i_count = lastIns; 
		join_pair.push_back(make_pair(child, parent));
		cout << "join pair: " << child.tid << " " << child.i_count << " " << parent.tid << " " << parent.i_count << endl;
		if (threadFinished[0] != 1)
		{
			parentTls->currentVectorClock->receiveActionFromParent(tld->currentVectorClock, threadid);
			parentTls->currentVectorClock->event();
		}
	        //parentTls->currentVectorClock->incEvent(threadid);
		
	}
	cout << "PIN: Thread Finished:" << threadid << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << semaphores[3].wait << semaphores[4].wait <<" " << &ctxt << " " <<ctxt <<" "<< code << " " << &v<< endl;
	    //if (threadid != 0)
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

	relax_element re;
	re.tid = threadid;
	re.i_count = tld->insCount;
	re.addr = lockMemoryWrite->effective_address;

	int zeroOneLock = tld->isEAXZero && tld->isZeroBefore && tld->isOneAfterAndUnlocked;
	int zeroTwoLock = tld->isEAXTwo && tld->isZeroBefore && tld->isTwoAfter;
	if (zeroOneLock || zeroTwoLock) {
		PIN_GetLock(&GlobalLock, tld->threadId);
		cout << "Lock Detected" << endl;
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
		re.vc = tld->currentVectorClock;
		curr_lock[threadid].push_front(re);
		PIN_ReleaseLock(&GlobalLock);
	}
	int zeroOneUnlock = tld->isOneBeforeAndLocked && tld->isZeroAfter;
	int twoOneUnlock = tld->isTwoBefore && tld->isOneAfterAndLocked;
	int twoZeroUnlock = tld->isTwoBefore && tld->isZeroAfter;

	if (zeroOneUnlock || twoOneUnlock || twoZeroUnlock) {
		relax_element pop_re;
		if (!curr_lock[threadid].empty())
		{
			pop_re = curr_lock[threadid].front();
			curr_lock[threadid].pop_front();
		}
		PIN_GetLock(&GlobalLock, tld->threadId);
		tld->out << "Unlocked" << endl;
		cout << "Unlocked" << endl;
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
		bool pushed = false;
		for (std::deque<pair<ADDRINT,deque <pair<relax_element, relax_element>>>>::iterator it = lock_addr_pair.begin(); it != lock_addr_pair.end(); ++it)
		{
			if(it->first == lockMemoryWrite->effective_address)
			{
				it->second.push_back(make_pair(pop_re,re));
				pushed = true;
				break;
			}
		}
		re.vc = tld->currentVectorClock;
		if (!pushed)
		{
			deque <pair<relax_element,relax_element>> lock_pair;
			lock_pair.push_back(make_pair(pop_re,re));
			lock_addr_pair.push_back(make_pair(re.addr,lock_pair));
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

bool hasStarted(THREADID tid)
{
	if (threadStarted[tid] == 1)
		return true;
	else
		return false;
}


bool isLockAddr(ADDRINT addr)
{
	for (std::deque<lockInfo>::iterator ad = lockAddr.begin(); ad != lockAddr.end(); ++ad)
	{
		//cout << "Lock address had addr " << ad->addr << endl;
		if (addr == ad->addr)
			return true;
	}
	return false;
}

VOID incrementThreadINS(THREADID tid, ADDRINT ins_addr, INS ins, CONTEXT *ctxt, int size, ADDRINT ip) {
	cnt++;
	ThreadLocalData *tld = getTLS(tid);
	tld->insCount++;
	cout << "address " << INS_Address(ins) <<" "<< ip << endl;
	cout  << "PIN: " << std::dec <<tid << " " << tld->insCount << " "<<tld->insCount2<< " " << ins_addr << " " << stack_end << reached_breakpoint << done << " " << waited << executed << next_execute << " " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << " " <<semaphores[0].wait<<semaphores[1].wait <<semaphores[2].wait<< " " << waited << executed << endl;

    for (std::deque<startInfo>::iterator si_iter = startInfoMap.begin(); si_iter != startInfoMap.end(); ++si_iter)
    {
	if (si_iter->start_addr == ins_addr)
	    si_iter->tid = tid;
    }

	if (tld->insCount == 1)
	{
		for (std::deque<threadInfo>::iterator ti_iter = threadInfoMap.begin(); ti_iter != threadInfoMap.end(); ++ti_iter)
		{
			if (ti_iter->tid == tid)
			{
				ti_iter->init_addr = ins_addr;
			}
		}
	}
	    /*for (std::list<ADDRINT>::iterator ad = addresses.begin(); ad != addresses.end(); ++ad)
	    {
	            ADDRINT * addr_ptr1 = (ADDRINT*) *ad;
	    	    ADDRINT value_w1;
	            PIN_SafeCopy(&value_w1, addr_ptr1, sizeof(int));
	            cout << "INCR: checking values at address " << *ad <<" "<< value_w1 << " " << &value_w1 << endl;
	    }*/
	if (!first_run && !race && !done)
	{
		for (std::deque<state>::iterator si = stack.begin(); si != stack.end(); ++si)
		{
			if ((si->tid == tid) && (si->count == tld->insCount))
			{
				if (si->done && si->pro)
				{
					cout << "Actually using this" << endl;
					if (semaphores[tid].wait < 1)
					{
						semaphores[tid].wait++;
						sem_wait(&semaphores[tid].s);
					}
				}
			}
		}
	}
	if (reached_breakpoint && done && !released)
	{
		released = true;
		for (int t = 0; t < thread_count; t++)
		{
			if (semaphores[t].wait > 0)
			{
				cout << "posting five " << t << endl;
				semaphores[t].wait--;
				sem_post(&semaphores[t].s);
			}
		}
	}
	if (race && !first_run)
	{
		if (!reached_breakpoint)
		{
			bool in_relax = false;
			if ((tid == tid2) && (count2 == tld->insCount))
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid2) && (it->count1 == count2) && (it->executed1))
					{
						in_relax = true;
						break;
					}
				}
				if (in_relax)
				{
					if (!executed)
					{
						if (semaphores[curr_state.tid].wait > 0)
						{
							semaphores[curr_state.tid].wait--;
							sem_post(&semaphores[curr_state.tid].s);
						}
					}
					if (executed && !next_execute)
					{
						if (semaphores[next_state.tid].wait > 0)
						{
							semaphores[next_state.tid].wait--;
							sem_post(&semaphores[next_state.tid].s);
						}
					}
					if (semaphores[tid].wait < 1)
					{
						semaphores[tid].wait++;
						sem_wait(&semaphores[tid].s);
					}
				}
			}
		}
	        /*if ((race) && (reached_breakpoint) && (stack_end) && (!done))
	        {
	            for (std::deque<relax_element>::iterator it = racepoint_sleep.begin(); it != racepoint_sleep.end(); ++it)
	            {
	                if ((it->tid == tid) && (it->i_count == tld->insCount) && (tid1 != tid) && (count1 != tld->insCount))
	                {
	                    if (semaphores[tid2].wait > 0)
	                    {
	                        cout << "posting eight " << tid2 << endl;
	                        semaphores[tid2].wait--;
	                        sem_post(&semaphores[tid2].s);
	                    }
	                    if (semaphores[tid].wait < 1)
	                    {
	                        cout << "waiting" << tid1 << tld->insCount << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
	                        semaphores[tid].wait++;
	                        sem_wait(&semaphores[tid].s);
	                    }
	                }
	            }
	        }*/
		if ((!reached_breakpoint) && (!done))
		{
			bool passRelax = false;
			if ((tid == tid1) && (count1 == tld->insCount))
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid1) && (it->count1 == count1) && (!it->executed1))
					{
						passRelax = true;
						break;
					}
				}
				if ((semaphores[tid].wait < 1) && (!passRelax))
				{
					cout << "First state will wait " << tid << endl;
					semaphores[tid].wait++;
					sem_wait(&semaphores[tid].s);
				}
			}
		}
		if ((reached_breakpoint) && (!done))
		{
			if (!((tid == tid2) && (count2 == tld->insCount)))
			{
				if ((tid == tid1) && (count1 == tld->insCount))
				{
					if (hasStarted(tid2))
					{
						if (semaphores[tid2].wait > 0)
						{
							cout << "posting six " << tid2 << endl;
							semaphores[tid2].wait--;
							sem_post(&semaphores[tid2].s);
						}
					}
					else
					{
						int t1;
						bool release;
						for (int t = 0; t < thread_count; t++)
						{
							t1 = t;
							release = true;
							for (std::deque<relax_element>::iterator it = racepoint_sleep.begin(); it != racepoint_sleep.end(); ++it)
							{
								if (it->tid == t)
								{
									release = false;
									break;
								}
							}
							if ((release) && (hasStarted(t)) && (t != tid1))
								break;
						}
						if ((release) && (hasStarted(t1)) && (t1 != tid1))
						{
							if (semaphores[t1].wait > 0)
							{
								cout << "posting seven " << t1 << endl;
								semaphores[t1].wait--;
								sem_post(&semaphores[t1].s);
							}
						}
					}
					if (semaphores[tid].wait < 1)
					{
						cout << "waiting here " << tid1 << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
						semaphores[tid].wait++;
						sem_wait(&semaphores[tid].s);
					}
				}
			}
		}
	}
	    if (/*(!reached_breakpoint)&&*/(!done) && (!first_run))
	{
		bool pre_relax = false;
		for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
		{
			bool in_ea = false;
			if ((it->tid1 == tid) && (it->count1 == tld->insCount) && (!it->executed1))
			{
				pre_relax = true;
				break;
			}
			if ((it->tid1 == tid) && (it->count1 == tld->insCount) && (it->executed1))
			{
				cout << "Enter Will execute late" << break_point.tid << endl;
				for (std::deque<state>::iterator e_a = exec_after.begin(); e_a != exec_after.end(); ++e_a)
				{
					if ((!done) && (e_a->tid == tid) && (e_a->count == tld->insCount) && (!(tid == tid1) && (tld->insCount == count1)))
					{
						in_ea = true;
						cout << "Will execute late" << tid << " " << semaphores[tid].wait << endl;
						if ((reached_breakpoint) && (!done))
						{
							if (hasStarted(tid2))
							{
								if (semaphores[tid2].wait > 0)
								{
									cout << "Releasing TID2 " << tid2 << endl;
									semaphores[tid2].wait++;
									sem_post(&semaphores[tid2].s);
								}
							}
							else
							{

							}
						}
						if (semaphores[tid].wait < 1)
						{
							cout << "waiting for exec after" << break_point.tid << endl;
							semaphores[tid].wait++;
							sem_wait(&semaphores[tid].s);
						}
					}
				}
				if (!reached_breakpoint)
				{
					if ((!waited && executed) && (!((next_state.tid == tid) && (next_state.count == tld->insCount))))
					{
						cout << "waiting for next state " << next_state.tid << endl;
						semaphores[tid].wait++;
						sem_wait(&semaphores[tid].s);  
					}
					if ((!waited && !executed) && (!((curr_state.tid == tid) && (curr_state.count == tld->insCount))))
					{
						if(!((curr_state.tid == tid) && (!in_ea)))
						{
							cout << "waiting for curr state " << next_state.tid << endl;
							semaphores[tid].wait++;
							sem_wait(&semaphores[tid].s);  
						}
					}
				}
			}
		}
		if (!pre_relax)
		{
			for (std::deque<state>::iterator it = exec_after.begin(); it != exec_after.end(); ++it)
			{
				if ((it->tid == tid) && (it->count == tld->insCount))
				{
					if (!reached_breakpoint)
					{
						if (((break_point.tid == curr_state.tid) && (!executed)) || ((break_point.tid == next_state.tid) && (executed && !waited)))
						{
							if (semaphores[break_point.tid].wait > 0)
							{
								cout << "posting not reached bp " << break_point.tid << endl;
								semaphores[break_point.tid].wait--;
								sem_post(&semaphores[break_point.tid].s);
							}
						}
					}
					cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx BP: " << reached_breakpoint  << " tid: " <<tid<< endl;

					if ((tid2 == tid) && (count2 == tld->insCount))
					{
						if ((semaphores[tid].wait < 1) && (!reached_breakpoint))
						{
							cout << "waiting " << tid1  << curr_state.tid << next_state.tid << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
							semaphores[tid].wait++;
							sem_wait(&semaphores[tid].s);
						}

					}
					else if ((tid2 == tid) && (count2 > tld->insCount) && (reached_breakpoint))
					{
						for (std::deque<state>::iterator ea = exec_after.begin(); ea != exec_after.end(); ++ea)
						{
							if ((ea->tid == tid) && (ea->count == tld->insCount))
							{
								in_ea = true;
								break;
							}
						}
					}
					else if (!in_ea)
					{
						if ((break_point.tid == tid) && (break_point.count == tld->insCount))
						{
							if (!reached_breakpoint)
							{}
					}
					else if (semaphores[tid].wait < 1)
					{
						if ((!executed) && (!((curr_state.tid == tid1) && (curr_state.count == count1) && (!done))))
						{
							if (semaphores[curr_state.tid].wait > 0)
							{
								cout << "posting one " << curr_state.tid << " "<< curr_state.count << " "<< tid << " " <<tld->insCount<< endl;
								semaphores[curr_state.tid].wait--;
								sem_post(&semaphores[curr_state.tid].s);
							}
						}
						else if (!waited)
						{
							if (semaphores[next_state.tid].wait > 0)
							{
								cout << "posting two " << next_state.tid << endl;
								semaphores[next_state.tid].wait--;
								sem_post(&semaphores[next_state.tid].s);
							}
						}
						else
						{
							bool can_post = true;
							if (stack.size() > 1)
							{
								if (hasStarted(stack[1].tid))
								{
									ThreadLocalData *top_tld = getTLS(stack[1].tid);
									if ((top_tld->insCount == count1) && (stack[1].tid == tid1) && (!done))
										can_post = false;
								}				
							}
							if ((semaphores[stack[1].tid].wait > 0) && (can_post))
							{
								cout << "posting three " << stack[1].tid << endl;
								semaphores[stack[1].tid].wait--;
								sem_post(&semaphores[stack[1].tid].s);
							}
						}
						cout << "waiting " << tid <<" "<< tid1 << curr_state.tid << next_state.tid << " " << executed << waited << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
						semaphores[tid].wait++;
						sem_wait(&semaphores[tid].s);
					}
				}
			}
		}
	}
}
	    /*Release locks on all threads if executed successfull till last inversion*/
if (!first_run)
{
	if ((((next_state.tid == 0) && (next_state.count == 0)) || ((stack_end) && (done)) ) || ((curr_state.tid == 0) && (curr_state.count == 0)))
	{
		for (int k = 0; k < thread_count; k++)
		{
			if (!((race) && (k == tid1) && (!post_t2)))
			{
				if (semaphores[k].wait > 0)
				{
					cout << "PIN: in release " << k << endl;
					semaphores[k].wait--;
					sem_post(&semaphores[k].s);
				}
			}
		}
	}
	        /* close if(!first_run)*/
	if ((stack_end) && (!done) && (tid != tid2) && (tld->insCount >= order[tid].front().count) && (order[tid].front().count > 0) && (!race))
	{
		bool cont = false;
		cout << "PIN: Other Wait " << tid << endl;
		for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
		{
			cout << "IDS " << it->tid1 << " " << it->count1 << it->count2 << endl;
			if (((tid == it->tid1) && (tld->insCount == it->count1)) || ((tid == it->tid2) && (tld->insCount == it->count2) && (it->executed1)) || ((tid == it->tid2) && (tld->insCount == it->count2 + 1) && (it->executed1) && (!it->executed2)))
			{
				cout << "CONT " << tid << endl;
				cont = true;
				break;
			}
		}
		for (std::deque<state>::iterator it = exec_after.begin(); it != exec_after.end(); ++it)
		{
			if ((it->tid == tid) && (tld->insCount == it->count))
			{
				cout << "CONT " << tid << endl;
				cont = false;
				break;
			}
		}
		if ((semaphores[tid].wait < 1) && (!cont))
		{
			cout << "WAIT CONT " << tid << endl;
			semaphores[tid].wait++;
			sem_wait(&semaphores[tid].s);
		}
	}
	if (stack_end)
	{
		bool dependent = false;
		if ((race) && (tid != tid2) && (!done) && (((tld->insCount >= order[tid].front().count) && (order[tid].front().count > 0)) || ((tld->insCount == count1) && (tid == tid1))))
		{
			cout << "PIN: WAIT " << tid << tld->insCount << " " << order[tid].front().count << endl;
			for (int i = 0; i < stack.size(); i++) {
				if ((stack[i].tid == tid) && (stack[i].tid != tid1) && (stack[i].count == tld->insCount))
				{
					dependent = true;
					break;
				}
				if ((stack[i].tid == tid2) && (stack[i].count == count2))
					break;
			}
			if (!dependent)
			{
				cout << "NOT Dependent" << endl;
				if ((semaphores[tid2].wait > 0) && (hasStarted(tid2)))
				{
					semaphores[tid2].wait--;
					sem_post(&semaphores[tid2].s);
				}
				if ((!hasStarted(tid2)))
				{
					for (int i = 0; i < stack.size(); i++) 
					{

						bool isExplored = false;
						for (std::deque<relax_element>::iterator rs = racepoint_sleep.begin(); rs != racepoint_sleep.end(); ++rs)
						{
							if (rs->tid == stack[i].tid)
							{
								if (hasStarted(stack[i].tid))
								{
									ThreadLocalData *rs_tld = getTLS(stack[i].tid);
									if (rs_tld->insCount == rs->i_count)
									{
										cout << "explored " << rs->tid  << " " << rs_tld->insCount << endl;
										isExplored = true;
										if ((tid == rs->tid) && (rs_tld->insCount == tld->insCount))
										{
											if ((semaphores[rs->tid].wait < 1) && (!stack_end))
											{
												cout << "explored waiting " << rs->tid  << " " << rs_tld->insCount << endl;
												semaphores[rs->tid].wait++;
												sem_wait(&semaphores[rs->tid].s);
												break;
											}
										}
										break;
									}
								}
							}
						}	

						if ((stack[i].tid != tid1) && (stack[i].tid != tid2) && (reached_breakpoint) && (!isExplored))
						{
							cout << "NOT Dependent diff thread " << stack[i].tid<< endl;
							if ((semaphores[stack[i].tid].wait > 0) && (hasStarted(stack[i].tid)))
							{
								cout << "NOT Dependent diff thread posting " << stack[i].tid<< endl;
								semaphores[stack[i].tid].wait--;
								sem_post(&semaphores[stack[i].tid].s);
								break;
							}
						}
					}
				}
				if ((semaphores[tid].wait < 1) && (hasStarted(tid2)) && (!stack_end))
				{
					cout << "NOT Dependent diff thread waiting " << tid<< endl;
					semaphores[tid].wait++;
					sem_wait(&semaphores[tid].s);
				}
				if ((tid == tid1) && (tld->insCount == count1) && (!done))
				{
					if (semaphores[tid].wait < 1) 
					{
						cout << "NOT Dependent diff thread waiting " << tid<< endl;
						semaphores[tid].wait++;
						sem_wait(&semaphores[tid].s);
					}
				}
			}
		}
		if ((race) && (tid == tid2) && (tld->insCount == count2) && (!done))
		{
			cout << "second state" << endl;
			if (!reached_breakpoint)
			{
				if (semaphores[tid2].wait < 1)
				{
					semaphores[tid2].wait++;
					sem_wait(&semaphores[tid2].s);
				}
			}
		}
		if ((race) && (tid == tid2) && (tld->insCount > count2) && (!done) && (second_done))
		{
			cout << "PIN: POST" << endl;
			done = true;
			for (int i = 0; i < thread_count; i++)
			{
				if (semaphores[i].wait > 0)
				{
					semaphores[i].wait--;
					sem_post(&semaphores[i].s);
				}
			}
		}
	}
	ADDRINT TakenIP = (ADDRINT)PIN_GetContextReg( ctxt, REG_INST_PTR );


	if ((!stack_end))
	{
		if ((curr_state.tid == next_state.tid))
		{
			PIN_LockClient();
			while ((curr_state.tid == next_state.tid) && (curr_state.count < next_state.count))
			{
				if (stack.size() >= 1)
				{
					cout << "PIN: same threads: changing to next" << endl;
					curr_state = next_state;
					stack.pop_front();
					order[curr_state.tid].pop_front();
					next_state = stack.front();
					executed = waited;
					waited = false;
				}
				else
				{
					stack_end = true;
					break;
				}
			}
			PIN_UnlockClient();
		}

		while ((curr_state.done) || (next_state.done))
		{
			cout << "*****************SWITCHING*********************" << endl;
			PIN_LockClient();
			if (curr_state.done)
			{
				curr_state = next_state;
				stack.pop_front();
				next_state = stack.front();
				order[curr_state.tid].pop_front();
			}
			if (next_state.done)
			{
				if (stack.size() > 1)
				{
					stack.pop_front();
					next_state = stack.front();
					order[next_state.tid].pop_front();
				}
				else
				{
					waited = true;
					stack_end = true;
					executed = true;
					next_execute = true;
					curr_state.tid = 0;
					next_state.tid = 0;
					curr_state.count = 0;
					next_state.count = 0;
					break;
				}
			}
			PIN_UnlockClient();
		}
		while ((order[curr_state.tid].front().count < curr_state.count) && (order[curr_state.tid].size() > 0) && (order[curr_state.tid].front().count > 0))
		{
			cout << "PIN: popping 1" << order[curr_state.tid].front().count << " " << curr_state.count << endl;
	                order[curr_state.tid].pop_front();  /*Pop for same threads*/
			cout << "PIN: Current top " << order[curr_state.tid].front().count << endl;
		}
		while ((order[next_state.tid].front().count < next_state.count) && (order[next_state.tid].size() > 0) && (order[next_state.tid].front().count > 0))
		{
			cout << "PIN: popping 2" << order[next_state.tid].front().count << " " << next_state.count << endl;
	                order[next_state.tid].pop_front(); /*Pop for same threads*/
			cout << "PIN: Next top " << order[next_state.tid].front().count << endl;
		}

		if ((((tld->insCount >= order[tid].front().count) && (curr_state.tid == tid) && (curr_state.count <= tld->insCount))) && (!executed))
		{
			bool isRelaxed = false;
			for (std::deque<relax_info>::iterator rf = relax_ds.begin(); rf != relax_ds.end(); ++rf)
			{
				if ((rf->tid1 == tid) && (rf->count1 < tld->insCount) && (rf->count1 == curr_state.count) && (rf->executed1))
				{
					cout << "Relaxed " << endl;
					isRelaxed = true;
					break; 
				}
			}
			if (!isRelaxed)
			{
				cout << "PIN: front of current state " << order[tid].front().count << " " << tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
				cout << "PIN: current tid " << tid << endl;
				pre_executed = true;
				order[tid].pop_front();
				sched_yield();
				cout << "PIN: top of order's current state " << order[tid].front().count << " " << tid << " " << curr_state.count << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
			}
		}
		if ((tid == next_state.tid) && (tld->insCount >= order[next_state.tid].front().count) && (!waited) && (order[next_state.tid].front().count != 0))
		{
			cout << "PIN: current pair " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
			cout << "PIN: waiting for next state " << tid << " " << tld->insCount << endl;
			cout << "PIN: order after waiting for next state " << tid << " " << order[tid].front().count << endl;
			string curr = std::to_string(curr_state.tid) + "_" + std::to_string(curr_state.count) + "_" + "r_{" + std::to_string(curr_state.tid) + "}_{" + std::to_string(curr_state.tid) + "}_[]_{}";
	                //cout << executed <<" "<< curr << endl;
	                if ((!executed) /*&& ((std::find(execution.begin(), execution.end(), curr) == execution.end()))*/)
			{
				cout << "PIN: before I am waiting " << semaphores[curr_state.tid].wait << endl;
				if (semaphores[curr_state.tid].wait > 0)
				{
					cout << "PIN: I am posting " << curr_state.tid << endl;
					semaphores[curr_state.tid].wait--;
					sem_post(&semaphores[curr_state.tid].s);
				}
				sched_yield();
				cout << "PIN: I am waiting " << tid << " " << tld->insCount << endl;
				if ((semaphores[tid].wait < 1) && (!executed) && (tid == next_state.tid))
				{
					semaphores[tid].wait++;
					sem_wait(&semaphores[tid].s);
				}
			}
			if ((tid == next_state.tid) && (tld->insCount >= order[next_state.tid].front().count) && (!waited) && (executed))
			{
				waited = true;
				PIN_LockClient();
				int top = order[tid].front().count;
				while (top == order[tid].front().count)
					order[tid].pop_front();
				PIN_UnlockClient();
			}
		}
		if (((tid == curr_state.tid) || (tid == next_state.tid)) && (tld->insCount >= order[tid].front().count) && (order[tid].front().count != 0))
		{
			if ((tid == next_state.tid) && (tld->insCount > next_state.count))
			{
				next_execute = true;
				cout << "Next Execute" << endl;
	                    // set true if the next state has already executed
			}
			cout << "PIN: Same thread waiting " << tid << " " << tld->insCount << " " << order[tid].front().count << curr_state.count << " " << next_state.count << endl;
	                next_tid = stack[1].tid; // assign the next active state *Check*
	                next_count = stack[1].count;
	                cout << "PIN: Same thread waiting post " << next_tid << " " << next_count << " " << order[tid].front().count << endl;
	                if (((tid == next_state.tid) || (tid == curr_state.tid)) && (tld->insCount == order[tid].front().count) && (tid == next_tid) && (tld->insCount == next_count) && (waited && executed))
	                {
	                	waited = false;
	                	executed = false;
	                	next_execute = false;
	                	curr_state = next_state;
	                	if (stack.size() <= 1)
	                	{
	                		stack_end = true;
	                	}
	                	stack.pop_front();
	                	stack.pop_front();
	                	curr_state = stack.front();
	                	stack.pop_front();
	                	next_state = stack.front();
	                	order[tid].pop_front();
	                	cout << "PIN: Next State same: Will not wait" << tid << " " << tld->insCount << " " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
	                	not_wait = true;
	                }
	                if (!executed)
	                {
	                	if (semaphores[curr_state.tid].wait > 0)
	                	{
	                		semaphores[curr_state.tid].wait--;
	                		sem_post(&semaphores[curr_state.tid].s);
	                	}
	                }
	                if (executed && !waited && !next_execute && !not_wait)
	                {
	                	if (semaphores[next_state.tid].wait > 0)
	                	{
	                		semaphores[next_state.tid].wait--;
	                		sem_post(&semaphores[next_state.tid].s);
	                	}
	                }
	                if ((waited) && (executed) && (next_execute) && (!not_wait))
	                {
	                	if (semaphores[next_tid].wait > 0)
	                	{
	                		semaphores[next_tid].wait--;
	                		sem_post(&semaphores[next_tid].s);
	                	}
	                }
	                if (tld->insCount > order[tid].front().count)
	                {
	                	for (std::deque<relax_info>::iterator rf = relax_ds.begin(); rf != relax_ds.end(); ++rf)
	                	{
	                		if ((rf->tid1 == tid) && (rf->count1 == order[tid].front().count))
	                		{
	                			not_wait = true;
	                			break;
	                		}
	                	}
	                }
	                if ((reached_breakpoint) && (!done) && (tid == tid2) && (tld->insCount == count2))
	                {
	                	not_wait = true;
	                }

	                if ((semaphores[tid].wait < 1) && (!not_wait))
	                {
	                	cout << "PIN: Same thread waiting :WAITS" << tid << " " << tld->insCount << " " << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
	                	semaphores[tid].wait++;
	                	sem_wait(&semaphores[tid].s);
	                }
	                not_wait = false;
	            }

	            if ((tid == next_state.tid) && (tld->insCount > next_state.count))
	            {
	            	next_execute = true;
	            	for (std::deque<state>::iterator ds = deleted_state.begin(); ds != deleted_state.end(); ++ds)
	            	{
	            		if ((ds->tid == next_state.tid) && (ds->count == next_state.count))
	            		{
	            			next_execute = false;
	            			break;
	            		}
	            	}
	            }
	            if (waited && executed && next_execute)
	            {
	            	next_execute = false;
	            	if (stack.size() <= 1)
	            	{
	            		if (wait_t1 && post_t2)
	            			stack_end = true;
	            	}
	            	stack.pop_front();
	            	curr_state = stack.front();
	            	stack.pop_front();
	            	next_state = stack.front();
	            	cout << "PIN: state changing " << curr_state.tid << curr_state.count << " " << next_state.tid << endl;
	            	waited = false;
	            	executed = false;
	            	cout << "PIN: " << curr_state.tid << " this is the new next state" << curr_state.count << endl;
	            	cout << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << endl;
	            	while (semaphores[curr_state.tid].wait > 0)
	            	{
	            		cout << "PIN: " << curr_state.tid << " Releasing locks on next state " << curr_state.count  << endl;
	            		semaphores[curr_state.tid].wait--;
	            		sem_post(&semaphores[curr_state.tid].s);
	            	}
	            }//    if(waited && executed)
	            if ((((tid != curr_state.tid) && (tid != next_state.tid) && (order[tid].front().count > 0) && (tld->insCount >= order[tid].front().count) && (!done)) || ((tid == next_state.tid) && (curr_state.tid != next_state.tid) && (!done) && (!executed) && (tld->insCount == next_state.count))) && (!reached_breakpoint))
	            {

	            	cout << "PIN: other thread waiting for next state " << tid << " " << tld->insCount << " " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << " " << semaphores[curr_state.tid].wait << semaphores[next_state.tid].wait << endl;
	            	if (!executed)
	            	{
	            		cout << "curt tid releasing in other thread " << curr_state.tid << endl;
	            		if (semaphores[curr_state.tid].wait > 0)
	            		{
	            			cout << "curt tid POST" << endl;
	            			semaphores[curr_state.tid].wait--;
	            			sem_post(&semaphores[curr_state.tid].s);
	            		}
	            	}
	            	if (executed && !waited)
	            	{
	            		cout << "next tid releasing in other thread" << endl;
	            		if (semaphores[next_state.tid].wait > 0)
	            		{
	            			cout << "next tid POST" << endl;
	            			semaphores[next_state.tid].wait--;
	            			sem_post(&semaphores[next_state.tid].s);
	            		}
	            		else
	            			sched_yield();
	            	}
	            	sched_yield();
	            	if (semaphores[tid].wait < 1)
	            	{
	            		cout << "thread waiting " << tid << endl;
	            		semaphores[tid].wait++;
	            		sem_wait(&semaphores[tid].s);
	            	}

	            }
	        }  // if((!reverse_point)&&(!stack_end))
	    }    //if((!first_run)&&(!finished)&&(!stack_end))
	}
	VOID MemoryReadInst(THREADID threadid, ADDRINT effective_address, ADDRINT read_addr, ADDRINT ins_addr, int i, bool preLock)
	{
		ADDRINT * addr_ptr = (ADDRINT*)effective_address;
		ADDRINT * addr_ptr1 = (ADDRINT*)read_addr;
		ADDRINT value_r, value_r1;
		PIN_SafeCopy(&value_r, addr_ptr, sizeof(int));
		PIN_SafeCopy(&value_r1, addr_ptr1, sizeof(int));
		cout << "Read value: " << value_r << " " << value_r1<< " " << &value_r << endl;
		bt_state b;
		relax_element read_element;
		list<MemoryAddr*>::const_iterator lookup1 =
		find_if(memSet.begin(), memSet.end(), mem_has_addr(effective_address));
		if (lookup1 == memSet.end())
		{
			MemoryAddr* mem = new MemoryAddr(effective_address);
			memSet.push_back(mem);
			writeIntersection.insert(effective_address);
		}
		list<MemoryAddr*>::const_iterator lookup =
		find_if(memSet.begin(), memSet.end(), mem_has_addr(effective_address));
		if (lookup != memSet.end()) {
			ThreadLocalData* tld = getTLS(threadid);
			PIN_GetLock(&GlobalLock, tld->threadId);
			cout << "PIN: Read " << threadid << " " << tld->insCount << " " << effective_address  << " " << ins_l << endl;
	        //sharedAccesses << tld->threadId << " " << tld->insCount << " r " << effective_address << "," << endl;
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

			if ((threadid == break_point.tid) && (tld->insCount == break_point.count))
			{
				bool in_wr = false;
				for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				{
					if ((wr->tid == threadid) && (wr->i_count1 == tld->insCount) && (!wr->executed1))
					{
						in_wr = true;
						break;                
					}
				}
				if (!in_wr)
				{
					cout << "In BP post" << endl;
					stack_end = true;
					reached_breakpoint = true;
					if (semaphores[tid2].wait > 0)
					{
						semaphores[tid2].wait--;
						sem_post(&semaphores[tid2].s);
					}
				}
			}

			for (int k = 0; k < size - 1; k++) {
				if ((*lookup)->accesses[k] == 'w') {
					if ((*lookup)->accessClocks[k].areConcurrent(&((*lookup)->accessClocks[size - 1]))) {
						instructions << "race " << (*lookup)->accessingThread[k] << " " << (*lookup)->accessingInstructions[k] << " " << (*lookup)->accessingThread[size - 1] << " " << (*lookup)->accessingInstructions[size - 1] << endl;
					}
				}
			}	   
			event = std::to_string(threadid) + "_" + std::to_string(tld->insCount) + "_" + "r_{" + std::to_string(threadid) + "}_{" + std::to_string(threadid) + "}_[]_{}";
			read_element.tid = threadid;
			read_element.vc = tld->currentVectorClock;
			read_element.ins = ins_l;
			read_element.addr = effective_address;
			read_element.i_count = tld->insCount;
			read_element.type = 'r';
			read_element.islock = preLock;
			b.event = read_element;
			PIN_LockClient();
			cout << "pushing in bt_table read " << threadid <<" "<< tld->insCount << endl;
			bt_table.push_back(b);
			PIN_UnlockClient();
			if ((tid1 == threadid) && (count1 == tld->insCount))
				state1 = read_element;
			if ((threadid == tid2) && (tld->insCount == count2))
			{
				cout << "Done" << endl;
				second_done = true;
				done = true;
			}
			for (std::deque<sleep_element>::iterator se = race_sleep.begin(); se != race_sleep.end(); ++se)
			{
				if ((se->tid == threadid) && (se->count == tld->insCount))	
				{
					se->vc = tld->currentVectorClock;
					se->ins = ins_l;
					se->addr = effective_address;
					se->type = 'r';
				}
			}
			for (std::vector<pair<THREADID, vector<relax_element>>>::iterator it = relax_struct.begin(); it != relax_struct.end(); ++it)
			{
				if (it->first == threadid)
				{
					int size = it->second.size();
					for (int k = size - 1; k >= size - window_size; k--)
					{
						if ((it->second[k].type == 'w')  && (effective_address != it->second[k].addr))
							instructions << "relax " << it->second[k].tid << " " << it->second[k].i_count << " " << threadid << " " << tld->insCount << endl;
						else
							break;
					}
					it->second.push_back(read_element);
				}
			}
			if (!first_run && !stack_end)
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->executed1) && (it->executed2) && (it->done) && (threadid == it->tid1) && (tld->insCount == it->count1))
					{
						cout << "update counts " << endl;
	                    //tld->insCount = it->count2; // error
					}
				}
				if (((pre_executed) || (tld->insCount == curr_state.count)) && (curr_state.tid == threadid) && (!executed))
				{
					executed = true;
					pre_executed = false;
					cout << "PIN: **************** PRE ExECUTE**************** " << stack_end << done << endl;
					if (!((next_state.tid == tid1) && (next_state.count == count1) && (!done)))
					{
						if (semaphores[next_state.tid].wait > 0)
						{
							semaphores[next_state.tid].wait--;
							sem_post(&semaphores[next_state.tid].s);
							cout << "PIN: **************** POSTING****************" << endl;
						}
					}
				}
				if ((waited) && (threadid == next_state.tid ) && (tld->insCount == next_state.count))
					next_execute = true;
				if (order[threadid].front().count == tld->insCount)
				{
					order[threadid].pop_front();
					cout << "popping " << next_state.tid << next_state.count << " " << tld->insCount << endl;
				}
				if ((threadid == curr_state.tid) && (tld->insCount == curr_state.count) && (!executed))
				{
					executed = true;
					cout << "executed in readinst " << waited << executed << endl;
					if (semaphores[next_state.tid].wait > 0)
					{
						cout << "posting four " << next_state.tid << endl;
						semaphores[next_state.tid].wait--;
						sem_post(&semaphores[next_state.tid].s);
					}
				}
	            if ((threadid == next_state.tid) && (tld->insCount == next_state.count) /*&& (!waited)*/)
				{
					cout << "waited in readinst " << tld->insCount << endl;
					waited = true;
					next_execute = true;
				}
				if (waited && executed && next_execute)
				{
					cout << "switching in read inst " << curr_state.tid << curr_state.count << " " << next_state.tid << next_state.count << " " << done << endl;
					waited = false;
					executed = false;
					next_execute = false;
					curr_state = next_state;
					if (stack.size() <= 1)
					{
						stack_end = true;
					}
	                //stack.pop_front();
					stack.pop_front();
					curr_state = stack.front();
					stack.pop_front();
					next_state = stack.front();
					cout << "new states " << curr_state.tid << curr_state.count << " " << next_state.tid << next_state.count << " " << done << endl;
					cout << semaphores[0].wait << semaphores[1].wait << semaphores[2].wait << semaphores[3].wait << semaphores[4].wait << endl;
					if (semaphores[curr_state.tid].wait > 0)
					{
						cout << "posting four " << curr_state.tid << endl;
						semaphores[curr_state.tid].wait--;
						sem_post(&semaphores[curr_state.tid].s);
					}
				}
			}
		}
		PIN_ReleaseLock(&((*lookup)->MemoryLock));
	}

	VOID MemoryWriteInst(THREADID threadid, ADDRINT effective_address, ADDRINT * write_addr, ADDRINT ins_addr, int i, bool preLock) {
		writeRelax write_relax;
		bool skipRelax = false;
		if (std::find(addresses.begin(), addresses.end(), effective_address) == addresses.end())
		{
			cout << "Addring to addreses: " << effective_address << endl;
			addresses.push_back(effective_address);
		}

		ThreadLocalData* tld = getTLS(threadid);
		for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
		{
			if ((it->tid1 == threadid) && (it->count1 == tld->insCount))
			{
				write_relax.tid = threadid;
				write_relax.i_count1 = it->count1;
				write_relax.i_count2 = it->count2;
				write_relax.ins_addr = ins_addr;
				write_relax.memOp = effective_address;
				ADDRINT * addr_ptr = (ADDRINT*)effective_address;
				ADDRINT value_w;
				PIN_SafeCopy(&value_w, addr_ptr, sizeof(int));
				write_relax.prev_value = value_w;
				cout << "Write value: " << value_w <<" " << &value_w  << endl;
				writeRelaxQueue.push_back(write_relax);
			}
		}
		for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
		{
			if ((it->tid1 == threadid) && (it->count1 == tld->insCount))
			{
				skipRelax = true;
			}
		}

		bt_state b;
		relax_element write_element;
		list<MemoryAddr*>::const_iterator lookup1 =
		find_if(memSet.begin(), memSet.end(), mem_has_addr(effective_address));
		if (lookup1 == memSet.end())
		{
			MemoryAddr* mem = new MemoryAddr(effective_address);
			memSet.push_back(mem);
			writeIntersection.insert(effective_address);
		}
		list<MemoryAddr*>::const_iterator lookup =
		find_if(memSet.begin(), memSet.end(), mem_has_addr(effective_address));
		if (lookup != memSet.end()) {
			PIN_GetLock(&GlobalLock, tld->threadId);
	        //sharedAccesses << tld->threadId << " " << tld->insCount << " w " << effective_address << "," << endl;
			cout << "PIN: write " << skipRelax<<" "<< threadid << " " << tld->insCount  << " " << effective_address << " " << ins_s << endl;
			PIN_ReleaseLock(&GlobalLock);
			tld->addAddressToLockWrite(effective_address);
			if (!skipRelax)
				tld->currentVectorClock->event();
			PIN_GetLock(&((*lookup)->MemoryLock), tld->threadId);
			(*lookup)->accesses.push_back('w');
			(*lookup)->operand_index.push_back(i);
			(*lookup)->accessingThread.push_back(threadid);
			(*lookup)->accessingInstructions.push_back(tld->insCount);
			(*lookup)->accessClocks.push_back(*(tld->currentVectorClock));
			int size = (*lookup)->accesses.size();

			if ((threadid == break_point.tid) && (tld->insCount == break_point.count))
			{
				bool in_wr = false;
				for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				{
					if ((wr->tid == threadid) && (wr->i_count1 == tld->insCount) && (!wr->executed1))
					{
						in_wr = true;
						break;                
					}
				}
				if (!in_wr)
				{
					cout << "In BP post write" << endl;
					stack_end = true;
					reached_breakpoint = true;
					if (semaphores[tid2].wait > 0)
					{
						semaphores[tid2].wait--;
						sem_post(&semaphores[tid2].s);
					}
				}
			}

			for (int k = 0; k < size - 1; k++) {
				if ((*lookup)->accessClocks[k].areConcurrent(&((*lookup)->accessClocks[size - 1]))) {
					instructions << "race " << (*lookup)->accessingThread[k] << " " << (*lookup)->accessingInstructions[k] << " " << (*lookup)->accessingThread[size - 1] << " " <<   (*lookup)->accessingInstructions[size - 1] << endl;
				}
			}
			event = std::to_string(threadid) + "_" + std::to_string(tld->insCount) + "_" + "w_{" + std::to_string(threadid) + "}_{" + std::to_string(threadid) + "}_[]_{}";
			write_element.tid = threadid;
			write_element.vc = tld->currentVectorClock;
			write_element.ins = ins_s;
			write_element.addr = effective_address;
			write_element.i_count = tld->insCount;
			write_element.type = 'w';
			write_element.islock = preLock;
			b.event = write_element;
			if (!skipRelax)
			{
				PIN_LockClient();
				cout << "pushing in bt_table " << threadid <<" "<< tld->insCount << endl;
				bt_table.push_back(b);
				PIN_UnlockClient();
			}
			if ((tid1 == threadid) && (count1 == tld->insCount))
				state1 = write_element;
			if ((threadid == tid2) && (tld->insCount == count2))
			{
				bool to_be_relaxed = false;
				bool in_relax_ds = false;
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
			   	    if ((it->tid1 == threadid) && (it->count1 == tld->insCount))
			   	    {
			   	        in_relax_ds = true;
			   	        if (!it->executed2)
			   	            to_be_relaxed = true;
			   	    }
					
				}
				if ((!in_relax_ds) || ((in_relax_ds) && (!to_be_relaxed)))
				{
				    cout << "Done" << endl;
				    second_done = true;
				    done = true;
				} 
				else
				{
				    cout << "Not Done: to be relaxed" << endl;
				}   
			}
			for (std::deque<sleep_element>::iterator se = race_sleep.begin(); se != race_sleep.end(); ++se)
			{
				if ((se->tid == threadid) && (se->count == tld->insCount))	
				{
					se->vc = tld->currentVectorClock;
					se->ins = ins_s;
					se->addr = effective_address;
					se->type = 'w';
				}
			}
			for (std::vector<pair<THREADID, vector<relax_element>>>::iterator it = relax_struct.begin(); it != relax_struct.end(); ++it) {
				if (it->first == threadid)
				{
					int size = it->second.size();
					for (int k = size - 1; k >= size - window_size; k--)
					{
						if ((it->second[k].type == 'w')  && (effective_address != it->second[k].addr))
							instructions << "relax " << it->second[k].tid << " " << it->second[k].i_count << " " << threadid << " " << tld->insCount << endl;
						else
							break;
					}
					it->second.push_back(write_element);
				}
			}
			if (!first_run && !stack_end)
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->executed1) && (it->executed2) && (it->done) && (threadid == it->tid1) && (tld->insCount == it->count1))
					{
	                    //tld->insCount = it->count2;
					}
				}
				if  (((pre_executed) || (tld->insCount == curr_state.count)) && (curr_state.tid == threadid) && (!executed))
				{
					executed = true;
					pre_executed = false;
					cout << "PIN: **************** PRE ExECUTE****************" << done << stack_end << endl;
					if (!((next_state.tid == tid1) && (next_state.count == count1) && (!done)))
					{
						if (semaphores[next_state.tid].wait > 0)
						{
							semaphores[next_state.tid].wait--;
							sem_post(&semaphores[next_state.tid].s);
							cout << "PIN: **************** POSTING****************" << endl;
						}
					}
				}
				if ((waited) && (threadid == next_state.tid ) && (tld->insCount == next_state.count))
					next_execute = true;
				if (order[threadid].front().count == tld->insCount)
				{
					order[threadid].pop_front();
					cout << "popping " <<  tld->insCount << endl;
				}
				if ((threadid == curr_state.tid) && (tld->insCount == curr_state.count) && (!executed))
				{
					cout << "executed in writeinst " << waited << executed << endl;
					executed = true;
					if (semaphores[next_state.tid].wait > 0)
					{
						semaphores[next_state.tid].wait--;
						sem_post(&semaphores[next_state.tid].s);
					}
				}
	            if ((threadid == next_state.tid) && (tld->insCount == next_state.count) /*&& (!waited)*/)
				{
					cout << "waited in writeinst" << endl;
					waited = true;
					next_execute = true;
				}
				if (waited && executed && next_execute)
				{
					cout << "switching in write inst" << endl;
					waited = false;
					executed = false;
					next_execute = false;
					curr_state = next_state;
					if (stack.size() <= 1)
					{
						stack_end = true;
					}
	                //stack.pop_front();
					stack.pop_front();
					curr_state = stack.front();
					stack.pop_front();
					next_state = stack.front();
					if ((semaphores[curr_state.tid].wait > 0) && (!executed))
					{
						semaphores[curr_state.tid].wait--;
						sem_post(&semaphores[curr_state.tid].s);
					}
					if ((semaphores[next_state.tid].wait > 0) && (!waited && executed))
					{
						semaphores[next_state.tid].wait--;
						sem_post(&semaphores[next_state.tid].s);
					}
				}
			}
		}
		PIN_ReleaseLock(&((*lookup)->MemoryLock));
	}

	void __BreakPoint(THREADID tid)
	{
		if ((break_point.tid == -100) && (break_point.count == -100))
			return;
		cout << "PIN: IN BREAKPOINT" << endl;
		bool set_break = false;
		ThreadLocalData *tld = getTLS(tid);
		if ((tid == break_point.tid) && (tld->insCount == break_point.count) && (!reached_breakpoint))
		{
			cout << "PIN: BREAKPOINT" << endl;
			reached_breakpoint = true;
			if (semaphores[tid2].wait > 0)
			{
				semaphores[tid2].wait--;
				sem_post(&semaphores[tid2].s);
			}
		}
		if ((tid == tid2) && (tld->insCount == count2 - 1))
		{
			std::deque<state>::iterator si = stack.begin();
			for (; si != stack.end(); ++si)
			{
				if ((curr_state.tid == tid) && (curr_state.count > tld->insCount + 1) && (!executed))
					break;
				if ((si->tid == tid) && (si->count > tld->insCount + 1 ) && (!si->done))
				{
					break;
				}
				bool in_relax = false;
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((tid == it->tid1) && (tld->insCount == it->count1))
					{
						in_relax = true;
						break;
					}
				}
				if ((si->tid == tid) && (si->count == tld->insCount + 1 ) && (in_relax))
				{
					if ((!si->done) && (!reached_breakpoint) && (break_point.tid != tid2))
					{
						cout << "PIN: list" << tid << tld->insCount << " " << curr_state.tid << curr_state.count << " " << next_state.tid << next_state.count << " " << waited << executed << si->done << endl;
						cout << "PIN: BREAKPOINT WAIT 2 " << tid << tid2 << endl;
						if (semaphores[tid2].wait < 1)
						{
							wait_at_break[1] = true;
							semaphores[tid2].wait++;
							sem_wait(&semaphores[tid2].s);
						}
					}
				}
			}
		}
		if ((tid == tid1) && (tld->insCount == count1 - 1) && (!first_run) && (!race) && (!reached_breakpoint)&& (count2 == count1 + 1))
		{
			if (semaphores[tid1].wait < 1)
			{
				semaphores[tid1].wait++;
				sem_wait(&semaphores[tid1].s);
			}
		}
		if ((tid == tid1) && (tld->insCount == count1) && (!first_run))
		{
			std::deque<state>::iterator si = stack.begin();
			for (; si != stack.end(); ++si)
			{
	            //cout << si->tid << " " << si->count <<endl;
				if ((curr_state.tid == tid) && (curr_state.count > tld->insCount + 1) && (!executed))
					break;
				if ((si->tid == tid) && (si->count > tld->insCount ) && (!si->done))
				{
					break;
				}
				if ((!race) && (!done) && (reached_breakpoint))
					{cout << "breaking" << endl; break;}
				if ((si->tid == tid) && (si->count == tld->insCount))
					{   if (! si->done)
						{
							cout << "PIN: BREAKPOINT WAIT 1 " << tid1 << endl;
							if (!reached_breakpoint)
							{
	                        // cout << "PIN: BREAKPOINT WAIT 1 " << tid1 << endl;
								if (semaphores[tid1].wait < 1)
								{
	                            // cout << "PIN: BREAKPOINT WAIT 1 " << tid1 << endl;
									wait_at_break[0] = true;
									semaphores[tid1].wait++;
									sem_wait(&semaphores[tid1].s);
								}
							}
						}
					}
				}
			}
			if ((reached_breakpoint) && (wait_at_break[0]) && ((done && race) || (!race)))
			{
				cout << "PIN: BREAKPOINT POST " << tid << endl;
				if (semaphores[tid1].wait > 0)
				{
					semaphores[tid1].wait--;
					sem_post(&semaphores[tid1].s);
				}
	        //reached_breakpoint = false;
				wait_at_break[0] = false;
			}
			if ((reached_breakpoint) && (wait_at_break[1]))
			{
				cout << "PIN: BREAKPOINT POST " << tid << endl;
				if (semaphores[tid2].wait > 0)
				{
					semaphores[tid2].wait--;
					sem_post(&semaphores[tid2].s);
				}
	        //reached_breakpoint = false;
				wait_at_break[1] = false;
			}
		}

		VOID AddMfence(THREADID tid) {
			ThreadLocalData *tld = getTLS(tid);
			fence_element fence;
			fence.tid = tid;
			fence.count = tld->insCount;
			fence_map.push_back(fence);
		}

		static ADDRINT returnValue (ADDRINT arg)
		{
			return arg;
		}


		void RecordReadAfter(THREADID tid, ADDRINT effective_address, ADDRINT ins_addr, int i)
		{
			relax_element write_element;
			bt_state b;
			for (std::list<ADDRINT>::iterator ad = addresses.begin(); ad != addresses.end(); ++ad)
			{
				ADDRINT * addr_ptr1 = (ADDRINT*) *ad;
				ADDRINT value_w1;
				PIN_SafeCopy(&value_w1, addr_ptr1, sizeof(int));
				cout << "Read after: checking values at address " << *ad <<" "<< value_w1 << " " << &value_w1 << endl;
			}
			ThreadLocalData *tld = getTLS(tid);
			if (reached_breakpoint && !done)
			{
			    for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
			    {
			        if ((it->tid1 == tid2) && (it->count1 == count2) && (!it->executed1) && (! done))
			        {
			            for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				    {
					if ((wr->tid == it->tid1) && (wr->i_count1 == it->count1) && (!wr->executed1))
					{
					    it->executed1 = true;
					    wr->executed1 = true;
					    ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					    ADDRINT * value_new = (ADDRINT*) &wr->value;
					    PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					    done = true;
					    second_done = true;
					    tld->currentVectorClock->event();
				            write_element.tid = wr->tid;
					    write_element.vc = tld->currentVectorClock;
					    write_element.addr = wr->memOp;
					    write_element.i_count = wr->i_count1;
					    write_element.type = 'w';
					    b.event = write_element;
					    bt_table.push_back(b);
					}
				    }		
			        }
			    }
			}
			
			if ((tid == tid2) && (tld->insCount == count2))
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid1) && (it->count1 == count1) && (!it->executed1))
					{
						for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
						{
							if ((wr->tid == it->tid1) && (wr->i_count1 == it->count1) && (!wr->executed1))
							{
								if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
								{
									stack_end = true;
									cout << "stack ended: read after" << endl;
								}
								it->executed1 = true;
								wr->executed1 = true;
								ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
								ADDRINT * value_new = (ADDRINT*) &wr->value;
								PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
								cout << "Read after: Write value after new: " <<wr->tid<<" "<< wr->i_count1 << " "<< value_new <<" " << wr->memOp  << endl;
								if ((wr->tid == tid2) && (wr->i_count1 == count2))
								{
								    cout << "Done" << endl;
								    second_done = true;
								    done = true;
								}
								tld->currentVectorClock->event();
								write_element.tid = wr->tid;
								write_element.vc = tld->currentVectorClock;
								write_element.addr = wr->memOp;
								write_element.i_count = wr->i_count1;
								write_element.type = 'w';
								b.event = write_element;
								bt_table.push_back(b);
								if (semaphores[wr->tid].wait > 0)
								{
									semaphores[wr->tid].wait--;
									sem_post(&semaphores[wr->tid].s);
								}
								break;
							}
						}
					}
				}
			}
			for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
			{
				if ((wr->tid == curr_state.tid) && (wr->i_count1 == curr_state.count) && (!executed))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
						{
							it->executed1 = true;
						}

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack ended: read after 2" << endl;
					}
					executed = true;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
				}
				if ((wr->tid == next_state.tid) && (wr->i_count1 == next_state.count) && (executed) && (!waited)&& (wr->executed2))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
							it->executed1 = true;

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "Stack ended: read after 3" << endl;
					}
					waited = false;
					executed = false;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: read after: " <<wr->tid<<" "<< wr->i_count1 <<" " << value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
					stack.pop_front();
					curr_state = stack.front();
					stack.pop_front();
					next_state = stack.front();
					cout << "switching " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
				}

				if ((wr->tid == tid) && (wr->i_count2 == tld->insCount))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid2 == tid) && (it->count2 == tld->insCount))
							it->executed2 = true;
					}
					wr->executed2 = true;
				}    
			}
			if (done && !flushAll)
			{
				flushAll = true;
				for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				{
					if (!wr->executed1)
					{
						if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
						{
							stack_end = true;
							cout << "stack end: read after 4" << endl;
						}
						wr->executed1 = true;
						ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
						ADDRINT * value_new = (ADDRINT*) &wr->value;
						PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
						cout << "flush read after: Write value after new: "<<wr->tid<<" "<< wr->i_count1 <<" "<< value_new <<" " << wr->memOp  << endl;
						if ((wr->tid == tid2) && (wr->i_count1 == count2))
						{
						    cout << "Done" << endl;
						    second_done = true;
						    done = true;
						}
						tld->currentVectorClock->event();
						write_element.tid = wr->tid;
						write_element.vc = tld->currentVectorClock;
						write_element.addr = wr->memOp;
						write_element.i_count = wr->i_count1;
						write_element.type = 'w';
						b.event = write_element;
						bt_table.push_back(b);
					}
				}
			}
			cout << "AFTER Read: " << tid << " " << tld->insCount << endl;
		}
		void RecordReadAtBranch(THREADID tid, ADDRINT effective_address, ADDRINT ins_addr, int i)
		{
			relax_element write_element;
			bt_state b;
			ThreadLocalData *tld = getTLS(tid);
			if ((tid == tid2) && (tld->insCount == count2))
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid1) && (it->count1 == count1) && (!it->executed1))
					{
						for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
						{
							if ((wr->tid == it->tid1) && (wr->i_count1 == it->count1) && (!wr->executed1))
							{
								if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
								{
									stack_end = true;
									cout << "stack end: read branch 1" << endl;
								}
								it->executed1 = true;
								wr->executed1 = true;
								ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
								ADDRINT * value_new = (ADDRINT*) &wr->value;
								PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
								cout << "Read branch: Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
								if ((wr->tid == tid2) && (wr->i_count1 == count2))
								{
								    cout << "Done" << endl;
								    second_done = true;
								    done = true;
								}
								tld->currentVectorClock->event();
								write_element.tid = wr->tid;
								write_element.vc = tld->currentVectorClock;
								write_element.addr = wr->memOp;
								write_element.i_count = wr->i_count1;
								write_element.type = 'w';
								b.event = write_element;
								bt_table.push_back(b);
								if (semaphores[wr->tid].wait > 0)
								{
									semaphores[wr->tid].wait--;
									sem_post(&semaphores[wr->tid].s);
								}
								break;
							}
						}
					}
				}
			}
			for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
			{
				if ((wr->tid == curr_state.tid) && (wr->i_count1 == curr_state.count) && (!executed))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
						{
							it->executed1 = true;
						}

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack end: read branch 2" << endl;
					}	    
					executed = true;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
				}
				if ((wr->tid == next_state.tid) && (wr->i_count1 == next_state.count) && (executed) && (!waited)&& (wr->executed2))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
							it->executed1 = true;

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack end: read branch 3" << endl;
					}
					waited = false;
					executed = false;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: read branch " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
					stack.pop_front();
					curr_state = stack.front();
					stack.pop_front();
					next_state = stack.front();
					cout << "switching " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
				}

				if ((wr->tid == tid) && (wr->i_count2 == tld->insCount))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid2 == tid) && (it->count2 == tld->insCount))
							it->executed2 = true;
					}
					wr->executed2 = true;
				}    
			}
			if (done && !flushAll)
			{
				flushAll = true;
				for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				{
					if (!wr->executed1)
					{
						if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
						{
							stack_end = true;
							cout << "stack end: read branch 4" << endl;
						}
						wr->executed1 = true;
						ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
						ADDRINT * value_new = (ADDRINT*) &wr->value;
						PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
						cout << "flush read branch: Write value after new: " <<wr->tid<<" "<< wr->i_count1 <<" "<< value_new <<" " << wr->memOp  << endl;
						if ((wr->tid == tid2) && (wr->i_count1 == count2))
						{
						    cout << "Done" << endl;
						    second_done = true;
						    done = true;
						}
						tld->currentVectorClock->event();
						write_element.tid = wr->tid;
						write_element.vc = tld->currentVectorClock;
						write_element.addr = wr->memOp;
						write_element.i_count = wr->i_count1;
						write_element.type = 'w';
						b.event = write_element;
						bt_table.push_back(b);
					}
				}
			}
			cout << "AT BRANCH Read: " << tid << " " << tld->insCount << endl;
		}
		void RecordWriteAfter(THREADID tid, ADDRINT effective_address, ADDRINT ins_addr, int i)
		{
			PIN_GetLock(&GlobalLock, -1);
			relax_element write_element;
			bt_state b;
			for (std::list<ADDRINT>::iterator ad = addresses.begin(); ad != addresses.end(); ++ad)
			{
				PIN_LockClient();
				ADDRINT * addr_ptr1 = (ADDRINT*) *ad;
				ADDRINT value_w1;
				PIN_SafeCopy(&value_w1, addr_ptr1, sizeof(int));
				cout << "write after: checking values at address " << *ad <<" "<< value_w1 << " " << &value_w1 << endl;
				PIN_UnlockClient();
			}
			ThreadLocalData* tld = getTLS(tid);
			if (reached_breakpoint && !done)
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid2) && (it->count1 == count2) && (!it->executed1) && (! done))
					{
						for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
						{
							if ((wr->tid == it->tid1) && (wr->i_count1 == it->count1) && (!wr->executed1))
							{
								it->executed1 = true;
								wr->executed1 = true;
								ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
								ADDRINT * value_new = (ADDRINT*) &wr->value;
								PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
								done = true;
								second_done = true;
								tld->currentVectorClock->event();
								write_element.tid = wr->tid;
								write_element.vc = tld->currentVectorClock;
								write_element.addr = wr->memOp;
								write_element.i_count = wr->i_count1;
								write_element.type = 'w';
								b.event = write_element;
								bt_table.push_back(b);
							}
						}		
					}
				}
			}
			
			if ((tid == tid2) && (tld->insCount == count2))
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid1) && (it->count1 == count1) && (!it->executed1))
					{
						for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
						{
							if ((wr->tid == it->tid1) && (wr->i_count1 == it->count1) && (!wr->executed1))
							{
								it->executed1 = true;
								wr->executed1 = true;
								ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
								ADDRINT * value_new = (ADDRINT*) &wr->value;
								for (std::list<ADDRINT>::iterator ad = addresses.begin(); ad != addresses.end(); ++ad)
								{
									ADDRINT * addr_ptr1 = (ADDRINT*) *ad;
									ADDRINT value_w1;
									PIN_SafeCopy(&value_w1, addr_ptr1, sizeof(int));
									cout << "before: checking values at address " << *ad <<" "<< value_w1 << " " << &value_w1 << endl;
								}
								PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
								cout << "write after: Write value after new: " <<wr->tid<<" "<< wr->i_count1<< " "<< &value_new << " "<<*value_new<<" " << wr->memOp <<" " << write_element.vc<< endl;
								if ((wr->tid == tid2) && (wr->i_count1 == count2))
								{
								    cout << "Done" << endl;
								    second_done = true;
								    done = true;
								}
								for (std::list<ADDRINT>::iterator ad = addresses.begin(); ad != addresses.end(); ++ad)
								{
									ADDRINT * addr_ptr1 = (ADDRINT*) *ad;
									ADDRINT value_w1;
									PIN_SafeCopy(&value_w1, addr_ptr1, sizeof(int));
									cout << "after: checking values at address " <<addr_ptr1 <<" "<< *ad <<" "<< value_w1 << " " << &value_w1 << endl;
								}
								tld->currentVectorClock->event();
								write_element.tid = wr->tid;
								write_element.vc = tld->currentVectorClock;

								write_element.addr = wr->memOp;
								write_element.i_count = wr->i_count1;
								write_element.type = 'w';
								b.event = write_element;
								bt_table.push_back(b);

								if (semaphores[wr->tid].wait > 0)
								{
									semaphores[wr->tid].wait--;
									sem_post(&semaphores[wr->tid].s);
								}
								break;
							}
						}
					}
				}
			}

			for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
			{
				if ((it->tid1 == tid) && (it->count1 == tld->insCount))
				{
					ADDRINT * addr_ptr = (ADDRINT*)effective_address;
					ADDRINT value_w;
					ADDRINT * value_prev = (ADDRINT*) writeRelaxQueue.back().prev_value;
					PIN_SafeCopy(&value_w, addr_ptr, sizeof(int));
					PIN_SafeCopy(addr_ptr, value_prev, sizeof(int));
					cout << "Write value after: " << value_w <<" " << effective_address  << endl;
					writeRelaxQueue.back().value = value_w;
				}
			}
			for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
			{
				cout << "check second instr" << endl;
				if ((wr->tid == curr_state.tid) && (wr->i_count1 == curr_state.count) && (!executed))
				{
					cout << "check second instr " << endl;
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
						{
							it->executed1 = true;
							break;
						}

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack end: write after 1" << endl;
					}
					executed = true;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
				}
				if ((wr->tid == next_state.tid) && (wr->i_count1 == next_state.count) && (executed) && (!waited) && (wr->executed2))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
							it->executed1 = true;

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack end: write after 2" << endl;
					}
					waited = false;
					executed = false;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: write after "<<wr->tid<<" "<< wr->i_count1<<" " << value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
					stack.pop_front();
					curr_state = stack.front();
					stack.pop_front();
					next_state = stack.front();
					cout << "switching " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
				}

				if ((wr->tid == tid) && (wr->i_count2 == tld->insCount))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid2 == tid) && (it->count2 == tld->insCount))
							it->executed2 = true;
					}
					wr->executed2 = true;
				}    
			}
			if (done && !flushAll)
			{
				flushAll = true;
				for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				{
					if (!wr->executed1)
					{
						if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
						{
							stack_end = true;
							cout << "stack end: write after 3" << endl;
						}
						wr->executed1 = true;
						ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
						ADDRINT * value_new = (ADDRINT*) &wr->value;
						PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
						cout << "flush write after: Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
						if ((wr->tid == tid2) && (wr->i_count1 == count2))
						{
						    cout << "Done" << endl;
						    second_done = true;
						    done = true;
						}
						tld->currentVectorClock->event();
						write_element.tid = wr->tid;
						write_element.vc = tld->currentVectorClock;
						write_element.addr = wr->memOp;
						write_element.i_count = wr->i_count1;
						write_element.type = 'w';
						b.event = write_element;
						bt_table.push_back(b);
					}
				}
			}
			PIN_ReleaseLock(&GlobalLock);
			cout << "AFTER WRITE: " << tid << " " << tld->insCount << endl;
		}
		void RecordWriteAtBranch(THREADID tid, ADDRINT effective_address, ADDRINT ins_addr, int i)
		{
			relax_element write_element;
			bt_state b;
			writeRelax write_relax;
			ThreadLocalData* tld = getTLS(tid);
			if ((tid == tid2) && (tld->insCount == count2))
			{
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == tid1) && (it->count1 == count1) && (!it->executed1))
					{
						for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
						{
							if ((wr->tid == it->tid1) && (wr->i_count1 == it->count1) && (!wr->executed1))
							{
								if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
								{
									stack_end = true;
									cout << "stack end: write branch 1" << endl;
								}
								it->executed1 = true;
								wr->executed1 = true;
								ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
								ADDRINT * value_new = (ADDRINT*) &wr->value;
								PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
								cout << "Write branch: Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
								tld->currentVectorClock->event();
								write_element.tid = wr->tid;
								write_element.vc = tld->currentVectorClock;
								write_element.addr = wr->memOp;
								write_element.i_count = wr->i_count1;
								write_element.type = 'w';
								b.event = write_element;
								bt_table.push_back(b);
								if (semaphores[wr->tid].wait > 0)
								{
									semaphores[wr->tid].wait--;
									sem_post(&semaphores[wr->tid].s);
								}
								break;
							}
						}
					}
				}
			}
			for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
			{
				if ((it->tid1 == tid) && (it->count1 == tld->insCount))
				{
					ADDRINT * addr_ptr = (ADDRINT*)effective_address;
					ADDRINT value_w;
					ADDRINT * value_prev = (ADDRINT*) writeRelaxQueue.back().prev_value;
					PIN_SafeCopy(&value_w, addr_ptr, sizeof(int));
					PIN_SafeCopy(addr_ptr, value_prev, sizeof(int));
					cout << "Write value after: " << value_prev <<" " << effective_address  << endl;
					writeRelaxQueue.back().value = value_w;
				}
			}
			for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
			{
				if ((wr->tid == curr_state.tid) && (wr->i_count1 == curr_state.count) && (!executed))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
						{
							it->executed1 = true;
						}

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack end: write branch 2" << endl;
					}
					executed = true;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" " << value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
				}
				if ((wr->tid == next_state.tid) && (wr->i_count1 == next_state.count) && (executed) && (!waited)&& (wr->executed2))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid1 == wr->tid) && (it->count1 == wr->i_count1))
							it->executed1 = true;

					}
					if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
					{
						stack_end = true;
						cout << "stack end: write branch 3" << endl;
					}
					waited = false;
					executed = false;
					wr->executed1 = true;
					ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
					ADDRINT * value_new = (ADDRINT*) &wr->value;
					PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
					cout << "Write value after new: write branch " <<wr->tid<<" "<< wr->i_count1<<" "<< value_new <<" " << wr->memOp  << endl;
					if ((wr->tid == tid2) && (wr->i_count1 == count2))
					{
					    cout << "Done" << endl;
					    second_done = true;
					    done = true;
					}
					tld->currentVectorClock->event();
					write_element.tid = wr->tid;
					write_element.vc = tld->currentVectorClock;
					write_element.addr = wr->memOp;
					write_element.i_count = wr->i_count1;
					write_element.type = 'w';
					b.event = write_element;
					bt_table.push_back(b);
					stack.pop_front();
					curr_state = stack.front();
					stack.pop_front();
					next_state = stack.front();
					cout << "switching " << curr_state.tid << " " << curr_state.count << " " << next_state.tid << " " << next_state.count << endl;
				}

				if ((wr->tid == tid) && (wr->i_count2 == tld->insCount))
				{
					for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
					{
						if ((it->tid2 == tid) && (it->count2 == tld->insCount))
							it->executed2 = true;
					}
					wr->executed2 = true;
				}    
			}
			if (done && !flushAll)
			{
				flushAll = true;
				for (std::deque<writeRelax>::iterator wr = writeRelaxQueue.begin(); wr != writeRelaxQueue.end(); ++wr)
				{
					if (!wr->executed1)
					{
						if ((wr->tid == break_point.tid) && (wr->i_count1 == break_point.count))
						{
							stack_end = true;
							cout << "stack end: write branch 4" << endl;
						}
						wr->executed1 = true;
						ADDRINT * addr_ptr = (ADDRINT*) wr->memOp;
						ADDRINT * value_new = (ADDRINT*) &wr->value;
						PIN_SafeCopy(addr_ptr, value_new, sizeof(int));
						cout << "flush write branch: Write value after new: " <<wr->tid<<" "<< wr->i_count1<<" " << value_new <<" " << wr->memOp  << endl;
						if ((wr->tid == tid2) && (wr->i_count1 == count2))
						{
						    cout << "Done" << endl;
						    second_done = true;
						    done = true;
						}
						tld->currentVectorClock->event();
						write_element.tid = wr->tid;
						write_element.vc = tld->currentVectorClock;
						write_element.addr = wr->memOp;
						write_element.i_count = wr->i_count1;
						write_element.type = 'w';
						b.event = write_element;
						bt_table.push_back(b);
					}
				}
			}
			cout << "AT BRANCH WRITE: " << tid << " " << tld->insCount << endl;
		}

		void 	rec_mem(INS ins) {
			cout << "Rec Enter" << endl;
			bool hasBranch = false;
			THREADID tid = PIN_ThreadId();
			ThreadLocalData *tld = getTLS(tid);
			int sz = INS_Size(ins);
			INS nextIns = INS_Next(ins);
			UINT32 next_size;
			if (INS_IsBranchOrCall(ins))
				hasBranch = true;
			if (INS_Valid(nextIns)) 
				next_size = INS_Size(nextIns);
			else 
				next_size = 0;
			tld->insCount2++;
			cout << "RECORD: " << tid << " " << tld->insCount2 << " " << tld->insCount <<" "<< INS_Address(ins) << endl;

			INS_InsertCall(ins,
				IPOINT_BEFORE,
				(AFUNPTR) __BreakPoint,
				IARG_THREAD_ID,
				IARG_END);

			INS_InsertCall(ins,
				IPOINT_BEFORE,
				(AFUNPTR) incrementThreadINS,
				IARG_THREAD_ID,
				IARG_ADDRINT, INS_Address(ins),
				IARG_PTR, ins,
				IARG_CONTEXT,
				IARG_UINT32, sz,
				IARG_INST_PTR,
				IARG_END);
			if (INS_Disassemble(ins).find("mfence") != std::string::npos)
			{
				INS_InsertCall(ins,
					IPOINT_BEFORE,
					(AFUNPTR) AddMfence,
					IARG_THREAD_ID,
					IARG_END);
			}

			cout << "Rec Exit " <<semaphores[0].wait<<semaphores[1].wait <<semaphores[2].wait<<semaphores[3].wait <<semaphores[4].wait<<  endl;
			if ((INS_IsStackRead(ins)) || (INS_IsStackWrite(ins)))
				return;
			UINT32 num_operands = INS_MemoryOperandCount(ins);
			UINT32 i;
			//UINT32 op_size;
			for (i = 0; i < num_operands; ++i) {
				//op_size =INS_MemoryOperandSize(ins,i);
				bool isLock = INS_LockPrefix(ins); 
				if (INS_MemoryOperandIsRead(ins, i)) {
					ins_l = INS_Disassemble(ins);

					INS_InsertCall(ins,
						IPOINT_BEFORE,
						(AFUNPTR) MemoryReadInst,
						IARG_THREAD_ID,
						IARG_MEMORYOP_EA, i,
						IARG_MEMORYREAD_EA,
						IARG_INST_PTR, 
						IARG_UINT32, i,
						IARG_BOOL, isLock,
						IARG_END);
					if (!hasBranch)
					{
						INS_InsertCall(ins,
							IPOINT_AFTER,
							(AFUNPTR) RecordReadAfter,
							IARG_THREAD_ID,
							IARG_MEMORYOP_EA, i,
							IARG_INST_PTR, 
							IARG_UINT32, i,
							IARG_END);
					}
					else
					{
						INS_InsertCall(ins,
							IPOINT_TAKEN_BRANCH,
							(AFUNPTR)RecordReadAtBranch,
							IARG_THREAD_ID,
							IARG_MEMORYOP_EA, i,
							IARG_INST_PTR, 
							IARG_UINT32, i,
							IARG_END);
					}
				}
				if (INS_MemoryOperandIsWritten(ins, i)) {

					ins_s = INS_Disassemble(ins);
					INS_InsertCall(ins,
						IPOINT_BEFORE,
						(AFUNPTR) MemoryWriteInst,
						IARG_THREAD_ID,
						IARG_MEMORYOP_EA, i,
						IARG_MEMORYWRITE_EA,
						IARG_INST_PTR, 
						IARG_UINT32, i,
						IARG_BOOL, isLock,
						IARG_END);

					if (!hasBranch)
					{
						INS_InsertCall(ins,
							IPOINT_AFTER,
							(AFUNPTR) RecordWriteAfter,
							IARG_THREAD_ID,
							IARG_MEMORYOP_EA, i,
							IARG_INST_PTR, 
							IARG_UINT32, i,
							IARG_END);
					}
					else
					{
						INS_InsertCall(ins,
							IPOINT_TAKEN_BRANCH,
							(AFUNPTR)RecordWriteAtBranch,
							IARG_THREAD_ID,
							IARG_MEMORYOP_EA, i,
							IARG_INST_PTR, 
							IARG_UINT32, i,
							IARG_END);
					}
				}
			}
		}

		VOID Pthread_create_callBefore( CHAR* name, pthread_t * lockaddr, ADDRINT pt1, pthread_t pt2, int i, ADDRINT addr, int j, CONTEXT * ctxt)
		{
			std::cout << std::hex << std::internal << std::setfill('0') 
	    << "pthread_create before RAX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RAX) << " " 
	    << "RBX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBX) << " " 
	    << "RCX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RCX) << std::endl
	    << "RDX = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RDX) << " " 
	    << "RDI = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RDI) << " " 
	    << "RSI = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RSI) << std::endl
	    << "RBP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RBP) << " "
	    << "RSP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RSP) << " "
	    << "RIP = " << std::setw(16) << PIN_GetContextReg(ctxt, LEVEL_BASE::REG_RIP) << std::endl;
	std::cout << "+-------------------------------------------------------------------" << std::endl;
			PIN_GetLock(&l, 1);
			ThreadLocalData *tld = getTLS(PIN_ThreadId());
			cout << lockaddr << " " << &lockaddr << " " << lockaddr[0]<< " " << lockaddr[1]<< " "  << pt1 << " " <<&pt1 << " " << pt2 << " " <<&pt2 << endl;
			startInfo si;
			si.start_addr = addr;
			si.start_count = tld->insCount;
			startInfoMap.push_back(si);
			cout << tld->insCount << endl;
			printf("Pthread_create_callBefore d %d %d %d %d %p %d \n ", lockaddr, pt1, pt2, i, addr, j);
			printf("Pthread_create_callBefore i %i\n", lockaddr);
			printf("Pthread_create_callBefore p %p %p %p %p %p %p\n", lockaddr , &pt1, &pt2, &i, &addr, &j);
			printf("Pthread_create_callBefore d %d\n", &lockaddr );
			printf("Pthread_create_callBefore i %i\n", &lockaddr);
			printf("Pthread_create_callBefore p %p\n", &lockaddr);
			PIN_ReleaseLock(&l);
		}

		VOID Pthread_create_callAfter(ADDRINT ret, pthread_t * t, pthread_t t1)
		{
			if(ret != 0)
				PIN_GetLock(&l, 2);
			printf("Pthread_create_callAfter d %d %d %d \n", ret, t, t1);
			printf("Pthread_create_callAfter i %d \n", ret );
			printf("Pthread_create_callAfter p %d \n", ret );
			printf("Pthread_create_callAfter d %d %p %p\n", &ret, &t, t1);
			printf("Pthread_create_callAfter i %d \n", &ret );
			printf("Pthread_create_callAfter p %d \n", &ret );
			PIN_ReleaseLock(&l);
		}



		VOID Pthread_mutex_lock_callBefore(CHAR* name, ADDRINT addr, THREADID tid, CONTEXT * ctxt)
		{
			bool hasAddr = false;
			lockInfo li;
			lockedRegion lr;
			ThreadLocalData *tld = getTLS(tid);
			lockInfo info;
			for (std::deque<lockInfo>::iterator ad = lockAddr.begin(); ad != lockAddr.end(); ++ad)
			{
				if (ad->addr == addr)
				{
					hasAddr = true;
					lr.tid = tid;
					lr.start = tld->insCount;
					ad->locked_region.push_back(lr);
					break;
				}
			}
			if (!hasAddr)
			{
				li.addr = addr;
				lr.tid = tid;
				lr.start = tld->insCount;
				li.locked_region.push_back(lr);
				lockAddr.push_back(li);
				cout << "Locked region Size at addr " << addr << " " << li.locked_region.size() << endl;
			}
			cout << "LOCK detected " << addr << " " << tid << " " << tld->insCount << " " << PIN_ThreadId() << endl; 
		}

		VOID Pthread_mutex_unlock_callBefore(CHAR* name, ADDRINT addr, THREADID tid, CONTEXT * ctxt)
		{
			cout << "Locked Region 4: " << tid << " " << addr  << endl; 
			ThreadLocalData *tld = getTLS(tid);
			for (std::deque<lockInfo>::iterator ad = lockAddr.begin(); ad != lockAddr.end(); ++ad)
			{
				cout << "Locked Region 3: " << tid << " " << addr << endl; 
				if (ad->addr == addr)
				{
					cout << "Locked Region 2: " << tid << " " << addr << endl; 
					for (std::deque<lockedRegion>::iterator l_rgn = ad->locked_region.begin(); l_rgn != ad->locked_region.end(); ++l_rgn)
					{
						cout << "Locked Region 1: " << tid <<" "<<l_rgn->tid<< " " << addr << " " << l_rgn->start << " " << l_rgn->end << endl; 
						if ((l_rgn->tid == tid) && (l_rgn->end == 0))
						{
							l_rgn->end = tld->insCount;
							cout << "Locked Region: " << tid << " " << addr << " " << l_rgn->start << " " << l_rgn->end << endl;  
						}
					}
				}
			}
			cout << "UNLOCK detected " << addr << " " << tid << " " << tld->insCount << " " << PIN_ThreadId() << endl; 
		}

		VOID Pthread_mutex_lock_callAfter(CHAR* name, ADDRINT addr, THREADID tid, CONTEXT * ctxt)
		{
			ThreadLocalData *tld = getTLS(tid);
			cout << "AFTER LOCK detected " << addr << " " << tid << " " << tld->insCount << " " << PIN_ThreadId() << endl; 
		}

		VOID Pthread_mutex_unlock_callAfter(CHAR* name, ADDRINT addr, THREADID tid, CONTEXT * ctxt)
		{
			ThreadLocalData *tld = getTLS(tid);
			cout << "AFTER UNLOCK detected " << addr << " " << tid << " " << tld->insCount << " " << PIN_ThreadId() << endl; 
		}
		VOID Pthread_join_callBefore(CHAR* name, pthread_t arg2, ADDRINT addr, pthread_t * pt, int i)
		{

			ThreadLocalData *tld = getTLS(PIN_ThreadId());
			
			cout << arg2 << " " << &arg2 << " " << pt[0] << " " << addr << " " << pt << " " <<&pt  << endl;

			for (std::deque<threadInfo>::iterator ti_iter = threadInfoMap.begin(); ti_iter != threadInfoMap.end(); ++ti_iter)
			{
				for (std::deque<startInfo>::iterator si_iter = startInfoMap.begin(); si_iter != startInfoMap.end(); ++si_iter)
				{
					if ((si_iter->start_addr == ti_iter->init_addr) && (!ti_iter->joined))
					{
						ti_iter->start = si_iter->start_count;
					}
				}
				if ((ti_iter->regAddr == addr) && (ti_iter->end == 0))
				{
					ti_iter->end = tld->insCount;
					ti_iter->joined = true;
					
				}
				

			}
			cout << "after end1" << endl; 

			printf("Pthread_join_callBefore d %d %d %d %d\n",arg2, i, addr, pt);
			
			printf("Pthread_join_callBefore i  %i\n", arg2);
			printf("Pthread_join_callBefore p  %p %p %p %p\n", arg2, &i , &addr, &pt);
			printf("Pthread_join_callBefore d  %d\n", &arg2);
			printf("Pthread_join_callBefore i  %i\n", &arg2);
			printf("Pthread_join_callBefore p  %p\n", &arg2);
						
			PIN_ReleaseLock(&l);
						
		}

		static VOID Pthread_join_callAfter(ADDRINT ret, pthread_t * t, pthread_t t1)
		{
					cout << "after end4" << endl; 
			if(ret != 0)
				return;
			PIN_GetLock(&l, 4);
			cout << t << " " << t1 <<  endl;
			printf("Pthread_join_callAfter d %d %d %d \n", ret, t, t1);
			printf("Pthread_join_callAfter i %d \n", ret );
			printf("Pthread_join_callAfter p %d \n", ret );
			printf("Pthread_join_callAfter d %d %p %p\n", &ret, &t, t1);
			printf("Pthread_join_callAfter i %d \n", &ret );
			printf("Pthread_join_callAfter p %d \n", &ret );
			
			PIN_ReleaseLock(&l);
		}


		VOID Image(IMG img, VOID *v)
		{
			RTN pmlRtn = RTN_FindByName(img, PTHREAD_CREATE);
			if (RTN_Valid(pmlRtn) && PIN_IsApplicationThread() )
			{
				RTN_Open(pmlRtn);
				cout << "Pthread Create " << IMG_Name(img) << endl;

				RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_create_callBefore, 
					IARG_ADDRINT, "pthread_create", IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0, IARG_FUNCARG_ENTRYPOINT_VALUE,
					0, IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0, IARG_FUNCARG_ENTRYPOINT_VALUE,
					1, IARG_FUNCARG_ENTRYPOINT_VALUE, 
					2, IARG_FUNCARG_ENTRYPOINT_VALUE, 3,
					IARG_CONTEXT,
					IARG_END);

	   /* RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_create_callBefore, 
	    IARG_ADDRINT, "pthread_create", IARG_FUNCARG_CALLSITE_REFERENCE, 
	    0, IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
	    IARG_FUNCARG_ENTRYPOINT_VALUE, 2, IARG_FUNCARG_ENTRYPOINT_VALUE, 3, IARG_END); */
	        /*RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_create_callBefore,
	                       IARG_SYSARG_REFERENCE,0,
	                       IARG_END);*/

	 //RTN_InsertCall(pmlRtn, IPOINT_BEFORE, AFUNPTR(Pthread_create_callBefore), IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER, IARG_G_ARG2_CALLER, IARG_G_ARG3_CALLER, IARG_END);

				RTN_InsertCall(pmlRtn, IPOINT_AFTER, (AFUNPTR)Pthread_create_callAfter,
					IARG_FUNCRET_EXITPOINT_VALUE,IARG_FUNCARG_ENTRYPOINT_VALUE,
					0,IARG_FUNCARG_ENTRYPOINT_VALUE,
					0,
					IARG_END);
				RTN_Close(pmlRtn);
			}
	    //pthread_mutex_unlock
			pmlRtn = RTN_FindByName(img, PTHREAD_JOIN);
			if (RTN_Valid(pmlRtn) )
			{
				cout << "Pthread Join" <<IMG_Name(img) << endl;
				RTN_Open(pmlRtn);

				RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_join_callBefore, 
					IARG_ADDRINT, "pthread_join", IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_FUNCARG_ENTRYPOINT_VALUE,
					0,IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_FUNCARG_ENTRYPOINT_VALUE, 
					1, IARG_END);
	   /* RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_join_callBefore, 
	    IARG_ADDRINT, "pthread_join", IARG_FUNCARG_CALLSITE_VALUE, 
	    0, IARG_FUNCARG_ENTRYPOINT_VALUE, 1,
	    IARG_END);*/
	       /*RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_join_callBefore,
	                       IARG_SYSARG_VALUE,0,
	                       IARG_END);*/
	 //RTN_InsertCall(pmlRtn, IPOINT_BEFORE, AFUNPTR(Pthread_join_callBefore), IARG_G_ARG0_CALLER, IARG_G_ARG1_CALLER,IARG_END);

				RTN_InsertCall(pmlRtn, IPOINT_AFTER, (AFUNPTR)Pthread_join_callAfter,
					IARG_FUNCRET_EXITPOINT_VALUE,IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_FUNCARG_ENTRYPOINT_VALUE,
					0, 
					IARG_END);
				RTN_Close(pmlRtn);

			}
			pmlRtn = RTN_FindByName(img, PTHREAD_MUTEX_LOCK);
			if (RTN_Valid(pmlRtn) )
			{
				RTN_Open(pmlRtn);
				RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_mutex_lock_callBefore, 
					IARG_ADDRINT, "pthread_mutex_lock", IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_THREAD_ID,
					IARG_CONTEXT, IARG_END);
				RTN_InsertCall(pmlRtn, IPOINT_AFTER, (AFUNPTR)Pthread_mutex_lock_callAfter, 
					IARG_ADDRINT, "pthread_mutex_lock", IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_THREAD_ID,
					IARG_CONTEXT, IARG_END);
				RTN_Close(pmlRtn);
			}
			pmlRtn = RTN_FindByName(img, PTHREAD_MUTEX_UNLOCK);
			if (RTN_Valid(pmlRtn) )
			{
				RTN_Open(pmlRtn);
				RTN_InsertCall(pmlRtn, IPOINT_BEFORE, (AFUNPTR)Pthread_mutex_unlock_callBefore, 
					IARG_ADDRINT, "pthread_mutex_unlock", IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_THREAD_ID,
					IARG_CONTEXT, IARG_END);
				RTN_InsertCall(pmlRtn, IPOINT_AFTER, (AFUNPTR)Pthread_mutex_unlock_callAfter, 
					IARG_ADDRINT, "pthread_mutex_unlock", IARG_FUNCARG_ENTRYPOINT_VALUE, 
					0,IARG_THREAD_ID,
					IARG_CONTEXT, IARG_END);
				RTN_Close(pmlRtn);
			}
		}



		VOID Trace(TRACE trace, VOID *val)
		{
			string img_name = "";
	    //if ((!IMG_IsMainExecutable(img)))
	    // return;
			PIN_GetLock(&GlobalLock, -1);
			ThreadLocalData *tld = getTLS(PIN_ThreadId());
			PIN_ReleaseLock(&GlobalLock);
			for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
			{
				SEC sec;
				IMG img;
				RTN rtn;
				for (INS ins = BBL_InsHead(bbl); INS_Valid(ins); ins = INS_Next(ins))
				{

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
					if ((!IMG_IsMainExecutable(img)) && (img_name.find("zlib") == std::string::npos))
						return;

					RTN pmlRtn = RTN_FindByName(img, PTHREAD_CREATE);
					if (RTN_Valid(pmlRtn) && PIN_IsApplicationThread() )
					{
						cout << "Image Name: " <<IMG_Name(img) << endl;
					}
					cout << img_name <<" " << INS_Disassemble(ins)<< endl;
					if (INS_IsAtomicUpdate(ins)) {
						check_lock(ins);
					}
					cout << "INS :: " << INS_Disassemble(ins) << endl;
					rec_mem(ins);
					tld->thread_trace << INS_Disassemble(ins) << endl;
				}
			}
			cout << "trace exit"  <<semaphores[0].wait<<semaphores[1].wait <<semaphores[2].wait<<semaphores[3].wait <<semaphores[4].wait << endl;
		}

		bool isRace(relax_element event1, relax_element event2)
		{
			cout << "Printing causally precedes: " <<causallyPrecedes(event1, event2) << endl;
			cout << "in israce " <<event1.tid << " " << event2.tid <<" "<< event1.type << " " << event2.type<<" "<< event1.addr << " " << event2.addr<<" " << event2.i_count<<" "<< event1.i_count << endl;
			if ((event1.tid!=event2.tid) && (!causallyPrecedes(event1, event2)) && (event1.type =='w' || event2.type =='w') && (event1.addr == event2.addr))
				return true;
			else
				return false;
		}

		bool isRelaxable(relax_element event1, relax_element event2)
		{
			bool non_relaxable = false;
			if (((event1.i_count < event2.i_count) && (event1.type == 'w')) || ((event1.i_count > event2.i_count) && (event2.type == 'w'))){
				if ((event1.tid == event2.tid) && (event2.i_count - event1.i_count < window_size)&& (event2.i_count - event1.i_count > (window_size * -1)) && (!event1.islock) && (!event2.islock) && (event1.addr != event2.addr))
				{
					for (std::deque<fence_element>::iterator fe = fence_map.begin(); fe != fence_map.end(); ++fe)
					{
						if ((fe->tid == event1.tid) && (fe->count > event1.i_count) && (fe->count < event2.i_count))
						{
							non_relaxable = true;
							break;
						}
					}
					if(!non_relaxable)
					{
						cout << "Relaxable " << event1.tid << " " << event1.i_count  << " " << event2.tid << " " << event2.i_count   << endl;
						return true;
					}
				}}
				cout << "Non Relaxable " << event1.tid << " " << event1.i_count  << " " << event2.tid << " " << event2.i_count   << endl; 
				return false;
			}

			VOID Fini(INT32 code, void *v)
			{
				bool has_initial = false;
				string temp = "";
				bool relax_present = false;
				bool added_relax = false;
				string instruction2 = std::to_string(tid2) + "_" + std::to_string(count2);
				cout << "********************* Thread Info Map **************************" << endl;
				for (std::deque <threadInfo>::iterator ti = threadInfoMap.begin(); ti != threadInfoMap.end(); ++ti)
				{
					cout << ti->tid << " " << ti->start << " " << ti->end <<" "<< ti->init_addr << endl;
				}
				list<MemoryAddr *>::const_iterator i;
				for (i = memSet.begin(); i != memSet.end(); i++) {
					cout << "PIN: **********************************" << endl;
					cout << "PIN: " << (*i)->addr << endl;

					int size = (*i)->accesses.size();
					int k;
					for (k = 0; k < size; k++) {
						cout << "PIN: " << k << ": " << endl;
						cout << "PIN: " << (*i)->accessingThread[k] << endl;
						cout << "PIN: " << (*i)->accessingInstructions[k] << endl;
						cout << "PIN: " << (*i)->accesses[k] << endl;

						int j;
						cout << "PIN: " ;
						for (j = 0; j < totalThreads; j++) {
							cout <<  (*i)->accessClocks[k].vclock_arr[j] << " ";
						}
						cout << endl;
					}
				}
				cout << std::dec <<"count " << cnt << endl;
				for (std::deque<threadInfo>::iterator ti_iter = threadInfoMap.begin(); ti_iter != threadInfoMap.end(); ++ti_iter)
				{
				    cout << ti_iter->tid << " " << ti_iter->start << " " << ti_iter->end << endl;
				}
				
				cout << "############## BT TABLE ################# " << bt_table.size()<< endl;
				for (int k = 198; k < bt_table.size(); k++)
				{
					cout << k <<" "<< bt_table[k].event.tid << " " << bt_table[k].event.i_count << endl;
				}
				cout << "############################### " << first_run << endl;
				if (!first_run)
				{
					for (int i = 0; i <= race_point; i++)
					{
						if (!bt_prev[i].sleep_set.empty())
						{
							for (int j = 0; j < bt_prev[i].sleep_set.size(); j++)
							{
								for (int k = 0; k < bt_table.size(); k++)
								{
									if ((bt_prev[i].sleep_set[j].tid == bt_table[k].event.tid) && (bt_prev[i].sleep_set[j].i_count == bt_table[k].event.i_count))
									{
										bt_prev[i].sleep_set[j].addr = bt_table[k].event.addr;
										bt_prev[i].sleep_set[j].type = bt_table[k].event.type;
										bt_prev[i].sleep_set[j].vc = bt_table[k].event.vc;
									}
								}
							}
						}
						if (!bt_prev[i].initials.empty())
						{
							for (int j = 0; j < bt_prev[i].initials.size(); j++)
							{
								for (int k = 0; k < bt_table.size(); k++)
								{
									if ((bt_prev[i].initials[j].tid == bt_table[k].event.tid) && (bt_prev[i].initials[j].i_count == bt_table[k].event.i_count))
									{
										bt_prev[i].initials[j].addr = bt_table[k].event.addr;
										bt_prev[i].initials[j].type = bt_table[k].event.type;
										bt_prev[i].initials[j].vc = bt_table[k].event.vc;
									}
								}
							}
						}
		   // if (i != race_point) 
						bt_table[i].sleep_set = bt_prev[i].sleep_set;
		    /*else
		    {
			if (!bt_prev[i].sleep_set.empty())
	                {
			    for (int j = 0; j < bt_prev[i].sleep_set.size(); j++)
	                    {
	                        if ((bt_prev[i].sleep_set[j].tid == tid1) && (bt_prev[i].sleep_set[j].i_count == count1))
				    bt_table[i].sleep_set.push_back(bt_prev[i].sleep_set[j]); 
				else if ((!(isRace(bt_prev[i].sleep_set[j],bt_table[i].event))) && ((bt_prev[i].sleep_set[j].tid != bt_table[i].event.tid)||(bt_prev[i].sleep_set[j].i_count != bt_table[i].event.i_count)))
				{
				   if(!isRelaxable(bt_table[i].event,bt_prev[i].sleep_set[j]))
				       bt_table[i].sleep_set.push_back(bt_prev[i].sleep_set[j]); 
				}
			    }
			}
		    }*/
						if ((i != race_point) || ((i == race_point) && (bt_table[i].event.tid == tid2) && (bt_table[i].event.i_count == count2)))
						{
							bt_table[i].initials = bt_prev[i].initials;
							for (std::deque<relax_element>::iterator in = bt_prev[i].initials.begin(); in != bt_prev[i].initials.end(); ++in)
							{
								bool has_sleep =false;
								if (!((isRelaxable(bt_table[i].event, *in)) || (isRace(bt_table[i].event, *in))))
								{
									for (std::deque<relax_element>::iterator ss_bt = bt_table[i].sleep_set.begin(); ss_bt != bt_table[i].sleep_set.end(); ++ss_bt)
									{
										if ((ss_bt->tid == in->tid) && (ss_bt->i_count == in->i_count))
										{
											has_sleep = true;
											break;
										}
									}
				//if (!has_sleep)
				  //  bt_table[i].sleep_set.push_back(*in);
								}
							}
						}
						else
						{
							for (std::deque<relax_element>::iterator in = bt_prev[i].initials.begin(); in != bt_prev[i].initials.end(); ++in)
							{
								bool has_init = false;
								if (!causallyPrecedes(bt_table[i].event, *in))
								{
									for (std::deque<relax_element>::iterator in_bt = bt_table[i].initials.begin(); in_bt != bt_table[i].initials.end(); ++in_bt)
									{
										if ((in_bt->tid == in->tid) && (in_bt->i_count == in->i_count))
										{
											has_init = true;
											break;
										}
									}
									if (!has_init)
										bt_table[i].initials.push_back(*in);
								}
							}
						}
						bt_table[i].relax_event = bt_prev[i].relax_event;
						bt_table[i].relaxed = bt_prev[i].relaxed;
					}
				}

	    /*Checked: ok*/
				for (std::deque<bt_state>::iterator es = bt_table.begin() + 1; es != bt_table.end(); ++es)
				{
					if ((!race) && (es->event.tid == tid2) && (es->event.i_count == count2) && (!added_relax) && (!first_run))
					{
						for (std::deque<relax_info>::iterator rxd = es->relaxed.begin(); rxd != es->relaxed.end(); ++rxd)
						{
							if ((tid1 == rxd->tid1) && (count1 == rxd->count1) && (tid2 == rxd->tid2) && (count2 == rxd->count2))
							{
								added_relax = true;
								break;
							}
						}
						if (!added_relax)
						{
							added_relax = true;
							relax_info ri1;
							ri1.tid1 = tid1;
							ri1.count1 = count1;
							ri1.tid2 = tid2;
							ri1.count2 = count2;
							es->relaxed.push_back(ri1);
						}
					}
	        /*Adding sleep element from reverse race*/
					if ((es->event.tid == tid2) && (es->event.i_count == count2))
					{
						bool hasSleep = false;
						for (std::deque<relax_element>::iterator ss = es->sleep_set.begin(); ss != es->sleep_set.end(); ++ss)
						{
							if ((ss->tid == tid1) && (ss->i_count == count1))
							{
								cout << "Reverse Sleepset not added" << endl;
								hasSleep = true;
								break;
							}
						}
						if ((!hasSleep) && (state1.i_count != 0) && (es->event.i_count != 0))
						{
							cout << "Reverse Sleepset added " << es->event.tid <<" "<< es->event.i_count <<" "<<state1.tid << " "<< state1.i_count<< endl;
							es->sleep_set.push_back(state1); 
						}
					}
	        /*checked ok*/
					for (std::deque<bt_state>::iterator es_pre = es - 1; es_pre >= bt_table.begin(); --es_pre)
					{   
	            /*Adding the relaxable events*/
						if ((es_pre->event.type == 'w') && (es->event.addr != es_pre->event.addr) && (es->event.tid == es_pre->event.tid) && (es->event.i_count - es_pre->event.i_count < window_size) && (!es_pre->event.islock) && (!es_pre->event.islock))
						{   
							relax_present = false;
							if (es_pre->relax_event.size() > 0)
							{
	                    /*Ignore if already present or has equivalent initial*/
								for (std::deque<relax_element>::iterator re = es_pre->relax_event.begin(); re != es_pre->relax_event.end(); ++re)
								{
									if ((re->tid == es->event.tid) && (re->i_count == es->event.i_count))
									{
										total1++;
										relax_present = true;
										break;
									}
									if ((re->addr == es->event.addr) || ((re->i_count < es->event.i_count) && (re->type == 'r')) || ((re->i_count > es->event.i_count) && (es->event.type == 'r')))
									{
										re->i_count = re->i_count < es->event.i_count ? re->i_count : es->event.i_count;
										cout << "added new: " << re->tid << " " << re->i_count << endl;
										cout << "added new pre: " << es_pre->event.tid << " " << es_pre->event.i_count << endl;
										cout << "added new next: " << es->event.tid << " " << es->event.i_count << endl;
										relax_present = true;
										break;
									}
								}
							}
	                /*Ignore if already present*/
							if ((es_pre->event.tid == tid2) && (es_pre->event.i_count == count2) && (es->event.tid == tid1) && (es->event.i_count == count1))
							{
								total1++;
								relax_present = true;
								break;
							}
			/*ignore if protected by mfence*/
							for (std::deque<fence_element>::iterator fe = fence_map.begin(); fe != fence_map.end(); ++fe)
							{
								if ((fe->tid == es->event.tid) && (fe->count > es_pre->event.i_count) && (fe->count < es->event.i_count))
								{
									relax_present = true;
									break;
								}
							}
							if (!relax_present)
							{
								total1++;
								if (es_pre->relax_event.empty())
									es_pre->relax_event = {}; 
								if (!((es_pre->event.tid == es->event.tid) && (es_pre->event.i_count == es->event.i_count)))
								{
									es_pre->relax_event.push_back(es->event);
									cout << "Relax: " <<  es_pre->event.tid << " " << es_pre->event.i_count << " " << es->event.tid << " " << es->event.i_count;
								}
							}
						}
	        /*Checked: ok*/ 
	        /*Building Sleepset from previous sleep set*/
						std::deque<bt_state>::iterator es_previous = es - 1;

						if (es_previous->sleep_set.size() > 0)
						{
							relax_element add_sleep;
							if (es - bt_table.begin() >= race_point)
							{
								cout << "***************** Check Sleep Exhaust ***************** " << es->event.tid <<" "<< es->event.i_count<< endl;

								bool has_raceSleep = false;
								for (std::deque<relax_element>::iterator in = es->sleep_set.begin(); in != es->sleep_set.end(); ++in)
								{
									for (std::deque<sleep_element>::iterator rs = race_sleep.begin(); rs != race_sleep.end(); ++rs)
									{
										if ((in->tid == rs->tid) && (in->i_count == rs->count))
										{
											cout << "has sleep: RELAXED before " << es->event.tid <<" "<< es->event.i_count << " " << rs->tid << " " << rs->count  << endl;
											rs->present = true;
											break;
										}
									}
								}

								for (std::deque<sleep_element>::iterator rs = race_sleep.begin(); rs != race_sleep.end(); ++rs)
								{
									if ((!rs->present) && (!rs->exhaust))
									{
										add_sleep.tid = rs->tid;
										add_sleep.i_count = rs->count;
										add_sleep.vc = rs->vc;
										add_sleep.addr = rs->addr;
										add_sleep.type = rs->type;
										cout << "has sleep: RELAXED :add " << es->event.tid <<" "<< es->event.i_count << " " << rs->tid << " " << rs->count << endl;
										es->sleep_set.push_back(add_sleep);
	                        //break;
									}
									else
										rs->present = false;
								}
								for (std::deque<sleep_element>::iterator rs = race_sleep.begin(); rs != race_sleep.end(); ++rs)
								{
									relax_element rs_relax;
									rs_relax.tid = rs->tid;
									rs_relax.i_count = rs->count;
									rs_relax.vc = rs->vc;
									rs_relax.addr = rs->addr;
									rs_relax.type = rs->type;
									if ((isRelaxable(es->event, rs_relax)) || (isRace(es->event, rs_relax))) 
									{
										cout << "has sleep: sleep exhaust " << es->event.tid <<" "<< es->event.i_count << " " << rs->tid << " " << rs->count<< endl;
										rs->exhaust = true;
									}
								}
							}
							for (std::deque<relax_element>::iterator ss = es_previous->sleep_set.begin(); ss != es_previous->sleep_set.end(); ++ss)
							{
	//error::(ss->vc->areConcurrent(es->event.vc))
			/*If there's a race: skip*/
								cout << "ss vc: " << ss->tid <<" " << ss->i_count << " "<< ss->vc << endl;
								cout << "es vc: " << es->event.tid << " " << es->event.i_count <<" "<<es->event.vc << endl;
								cout << "VC1: check" <<ss->vc->areConcurrent(es->event.vc)<< endl;//error
								if ((!((ss->vc->areConcurrent(es->event.vc)) && (ss->type == 'w' || es->event.type == 'w') && (ss->tid != es->event.tid) && (ss->addr == es->event.addr))) || ((es->event.tid == tid2) && (es->event.i_count == count2)) && (!(isLockAddr(ss->addr)))&& (!(isLockAddr(es->event.addr))))
								{
									bool has_sleep = false,present=false;
									for (std::deque<relax_element>::iterator in = es->sleep_set.begin(); in != es->sleep_set.end(); ++in)
									{
										if ((in->tid == ss->tid) && (ss->i_count == in->i_count))
										{
											cout << "has sleep: RELAXED " << ss->tid << " " << ss->i_count << endl;
											has_sleep = true;
											present = true;
											break;
										}
									}
									if (isRelaxable(*ss, es->event)) 
	/*((ss->tid == es->event.tid) && (ss->type == 'w' || es->event.type == 'w') && (ss->addr != es->event.addr) && (abs(ss->i_count - es->event.i_count) < window_size))*/
									{
										cout << "has sleep: RELAXABLE " << ss->tid << " " << ss->i_count << endl;
										has_sleep = true;
									}
									if (((es->event.tid == tid2) && (es->event.i_count == count2)) && (!present))
									{
										es->sleep_set.push_back(*ss);
										cout << "Adding sleep 1: " << ss->tid << " " << ss->i_count << endl;
									}
									else if ((!((ss->tid == es->event.tid) && (ss->i_count == es->event.i_count))) && (!has_sleep))
									{
										es->sleep_set.push_back(*ss);
										cout << "Adding sleep 2: " << ss->tid << " " << ss->i_count << endl;
									}
								}
								if ((ss->tid == tid1) && (ss->i_count == count1) && (es->event.tid == tid2) && (es->event.i_count == count2))
								{
									bool has_sleep = false;
									for (std::deque<relax_element>::iterator in = es->sleep_set.begin(); in != es->sleep_set.end(); ++in)
									{
										if ((in->tid == ss->tid) && (ss->i_count == in->i_count))
										{
											cout << "has sleep: OTHER " << ss->tid << " " << ss->i_count << endl;
											has_sleep = true;
											break;
										}
									}
									if ((!((ss->tid == es->event.tid) && (ss->i_count == es->event.i_count))) && (!has_sleep))
									{
										es->sleep_set.push_back(*ss);
										cout << "Adding sleep: " << ss->tid << " " << ss->i_count << endl;
									}
								}
							}
						}
						has_initial = false;
	            /*Adding initials*/
						if ((es->event.type == 'w' || es_pre->event.type == 'w') && (es->event.addr == es_pre->event.addr) && (es->event.tid != es_pre->event.tid) && (!(isLockAddr(es->event.addr))) && (!(isLockAddr(es_pre->event.addr))))
						{
							for (std::deque<relax_element>::iterator in = es_pre->sleep_set.begin(); in != es_pre->sleep_set.end(); ++in)
							{
								if ((causallyPrecedes(*in, es->event)) && (!isRelaxable(*in, es->event)))
								{
									has_initial = true;
									break;
								}
							}
							if (!causallyPrecedes(es->event, es_pre->event))
								cout << "Causally Precedes " <<es->event.tid <<" "<< es->event.i_count << " " <<es_pre->event.tid <<" "<< es_pre->event.i_count << endl;
							else
								cout << "has order " <<es->event.tid <<" "<< es->event.i_count << " " <<es_pre->event.tid <<" "<< es_pre->event.i_count << endl;
	                //if (es->event.vc->areConcurrent(es_pre->event.vc))
							if (!causallyPrecedes(es->event, es_pre->event))
							{
								cout << "ADDing Initials " << es->event.tid << " " << es->event.i_count << " " << es_pre->event.tid << " " << es_pre->event.i_count << endl;
								if (!es_pre->initials.empty())
								{
									for (std::deque<relax_element>::iterator in = es_pre->initials.begin(); in != es_pre->initials.end(); ++in)
									{
										if ((in->tid == es->event.tid))
										{
											int temp_min,temp_max;
					//cout << "Same tid exists" << endl;
					//total++;
											temp_min = in->i_count < es->event.i_count ? in->i_count : es->event.i_count;
											temp_max = in->i_count > es->event.i_count ? in->i_count : es->event.i_count;
											for (std::deque<relax_element>::iterator _in = es_pre->initials.begin(); _in != es_pre->initials.end(); ++_in)
											{
												if ((_in->tid == es->event.tid) && (_in->i_count == temp_min))
												{
													has_initial = true;
													break;
												}
											}
											if (!has_initial)
												in->i_count = in->i_count < es->event.i_count ? in->i_count : es->event.i_count;
											has_initial = true;
											break;
										}
										for (std::deque<bt_state>::iterator mid = es; mid > es_pre; --mid)
										{
											if ((in->tid == mid->event.tid) && (in->i_count == mid->event.i_count))
											{
					    //total++;
					    //cout << "Mid exixts" << endl;
												if (!((in->type == 'w' || mid->event.type == 'w') && (in->addr == mid->event.addr) && (in->tid != mid->event.tid))) 
												{
					        //cout << "independent mid exists" << endl;
													has_initial = true;
													break;
												}
											}
										}
									}
								}
								if (has_initial)
								{
									bool has_init = false;
									for (std::deque<relax_element>::iterator init = es_pre->initials.begin(); init != es_pre->initials.end(); ++init)
									{
										for (std::deque<relax_element>::iterator in = es_pre->initials.begin(); in != es_pre->initials.end(); ++in)
										{
											if ((in->tid == es->event.tid) && (in->i_count == es->event.i_count))
											{
												has_init = true;
												break;
											}
										}
										if (has_init)
											break;
										cout << "Adding Tricky initial out " << init->tid <<" "<<init->i_count<< endl;
										if ((isRace(*init, es_pre->event)) && (es_pre->event.type =='w') && (init->type == 'r'))
										{				  
											cout << "Adding Tricky initial in " << es->event.tid << " " << es->event.i_count << " "<<es_pre->event.tid << " " << es_pre->event.i_count << endl;
											if ((isRace(es_pre->event, es->event)) && (es->event.type == 'r'))
											{
												es_pre->initials.push_back(es->event);
												cout << "Adding Tricky initial" << endl;
												break;
											}
										}
									}
								}
								if (!has_initial)
								{
									if (es_pre->initials.empty())
									{
										if ((!causallyPrecedes(es->event, es_pre->event)))
											es_pre->initials.push_back(es->event);
									}
									else
									{
										for (std::deque<bt_state>::iterator initial = es_pre + 1; initial <= es; ++initial)
										{
		   		        //cout << "No initial exists " << endl;
	                                //if (es_pre->event.vc->areConcurrent(initial->event.vc))
											if (((!causallyPrecedes(initial->event, es_pre->event))) && (initial->event.tid == es->event.tid))
											{
												cout << "New initial added " << initial->event.tid << " " << initial->event.i_count<< endl;
												if (!((es_pre->event.tid == initial->event.tid) && (es_pre->event.i_count == initial->event.i_count)))
													es_pre->initials.push_back(initial->event);
												total++;
												has_initial = false;
												break;
											}
										}
									}
								}
							}
						}
					}
				}



				bt.open("backtrack.out");
				string bt_string = std::to_string(totalThreads) + "\n";
				int rp = 0;
				for (std::deque<bt_state>::iterator es = bt_table.begin(); es != bt_table.end(); ++es)
				{
					bt_string = bt_string + std::to_string(es->event.tid) + "_" + std::to_string(es->event.i_count) + "_" + es->event.type + "\n";
					bt_string = bt_string + "{";
					if ((rp == race_point) && (!((es->event.tid == tid2) && (es->event.i_count == count2))))
					{
						cout << "IN " << rp << endl;
						for (std::deque<relax_element>::iterator re = es->initials.begin() ; re != es->initials.end(); ++re)
						{
							cout << "INitials " << re->tid << re->i_count << endl;
							if (re->vc->areConcurrent(es->event.vc))
							{
								bt_string = bt_string + std::to_string(re->tid) + "_" + std::to_string(re->i_count) + "_" + re->type + ",";
							}
						}	
					}
					else
					{
						for (std::deque<relax_element>::iterator re = es->initials.begin() ; re != es->initials.end(); ++re)
						{
							bt_string = bt_string + std::to_string(re->tid) + "_" + std::to_string(re->i_count) + "_" + re->type + ",";
						}
					}
					if (bt_string.at(bt_string.length() - 1) == ',')
						bt_string = bt_string.substr(0, bt_string.length() - 1) + "}_{";
					else
						bt_string = bt_string + "}_{";
					for (std::deque<relax_element>::iterator ss = es->sleep_set.begin() ; ss != es->sleep_set.end(); ++ss)
					{
						bt_string = bt_string + std::to_string(ss->tid) + "_" + std::to_string(ss->i_count) + "_" + ss->type + ",";
					}
					if (bt_string.at(bt_string.length() - 1) == ',')
						bt_string = bt_string.substr(0, bt_string.length() - 1) + "}_<";
					else
						bt_string = bt_string + "}_<";
					if (((rp == race_point) && (es->event.tid == tid2) && (es->event.i_count == count2)) || (first_run) || (rp != race_point) || true)
					{
						for (std::deque<relax_element>::iterator rs = es->relax_event.begin() ; rs != es->relax_event.end(); ++rs)
						{
							if (bt_string.at(bt_string.length() - 1) != '<')
								bt_string = bt_string.substr(0, bt_string.find_last_of("<")+1) + std::to_string(rs->tid) + "_" + std::to_string(rs->i_count) + "_" + rs->type + "," + bt_string.substr(bt_string.find_last_of("<")+1);
							else
								bt_string = bt_string + std::to_string(rs->tid) + "_" + std::to_string(rs->i_count) + "_" + rs->type + ",";
						}
					}
					else 
					{
		    //cout << "Not adding" << rp << " "<< race_point << " " << tid2 << " " << count2 << " " << es->event.tid << " " << es->event.i_count << endl;
					}
					if (bt_string.at(bt_string.length() - 1) == ',')
						bt_string = bt_string.substr(0, bt_string.length() - 1) + ">_[";
					else
						bt_string = bt_string + ">_[";
					for (std::deque<relax_info>::iterator ri = es->relaxed.begin() ; ri != es->relaxed.end(); ++ri)
					{
						bt_string = bt_string + std::to_string(ri->tid1) + "_" + std::to_string(ri->count1) + "_"  + std::to_string(ri->tid2) + "_" + std::to_string(ri->count2) + ",";
					}
					if (bt_string.at(bt_string.length() - 1) == ',')
						bt_string = bt_string.substr(0, bt_string.length() - 1) + "]\n";
					else
						bt_string = bt_string + "]\n";
					rp++;
				}
				bt << bt_string << endl;
				bt.close();
				stop_s = clock();
				cout << "PIN: time: " << (stop_s - start_s) / double(CLOCKS_PER_SEC) * 1000 << endl;
				cout << "PIN: total race: " << total << endl;
				cout << "PIN: total relax: " << total1 << endl;

			}

			INT32 Usage()
			{
				cerr <<
				"PIN: This pin tool tries to find the locks and unlocks in a program.\n"
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
			{
				state st;
				deque<stack_element> dq = {};
				stack_element se;
				relax_info ri;
				int p = 0;
				std::ifstream file1("backtrack.out");
				FILE * pFile2;
				pFile2 = fopen ( "backtrack.out" , "r" );
				fseek(pFile2, 0, SEEK_END);
				if (ftell(pFile2) != 0)
	    {   /*if backtrack is not empty, record the previous execution stack*/
					string subs;
				std::getline(file1, subs);
				thread_count = std::stoi(subs);
				while (std::getline(file1, subs))
					{   bt_state bs;
						bs.sleep_set = {};
						if (subs != "")
						{
							st.tid = std::stoi(subs.substr(0, subs.find_first_of("_")));
							subs = subs.substr(subs.find_first_of("_") + 1);
							st.count = std::stoi(subs.substr(0, subs.find_first_of("_")));
							st.index = stack.size();
							stack.push_back(st);
							if (p == 0)
							{
								for (int r = 0; r < thread_count; r++)
								{
									order.push_back(dq);
								}
								p++;
							}
							if (!order[st.tid].empty())
							{
								if (order[st.tid].back().count > st.count)
								{
									exec_after.push_back(st);
									cout << "Exec Insert : " << st.tid <<" " << st.count <<" " << st.index<< endl;
								}
							}
							se.count = st.count;
							order[st.tid].push_back(se);
							bs.event.tid = st.tid;
							bs.event.i_count = st.count;
							bs.event.type = subs.at(subs.find_last_of("_") + 1);
						}
						std::getline(file1, subs);
						if (subs != "")
						{
							relax_element rx;
							relax_info ri;
							string temp = subs.substr(1, subs.find_first_of("}") - 1);
	                /*Reading Initials at state st in the previous run*/
							while (temp.find(",") != std::string::npos)
							{
								string t = temp.substr(temp.find_last_of(",") + 1);
								rx.tid = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								rx.i_count =  std::stoi(t.substr(0, t.find_first_of("_")));
								rx.type =  t.at(t.find_last_of("_") + 1);
								bs.initials.push_back(rx);
								temp = temp.substr(0, temp.find_last_of(","));
							}
							if (temp != "")
							{
								string t = temp;
								rx.tid = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								rx.i_count =  std::stoi(t.substr(0, t.find_first_of("_")));
								rx.type =  t.at(t.find_last_of("_") + 1);
								bs.initials.push_back(rx);
								temp = "";
							}
							subs = subs.substr(subs.find_first_of("}") + 2);
							temp = subs.substr(1, subs.find_first_of("}") - 1);
	                /*Reading Sleep_set at state st in the previous run*/
							while (temp.find(",") != std::string::npos)
							{
								string t = temp.substr(temp.find_last_of(",") + 1);
								rx.tid = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								rx.i_count =  std::stoi(t.substr(0, t.find_first_of("_")));
								rx.type =  t.at(t.find_last_of("_") + 1);
								bs.sleep_set.push_back(rx);
								temp = temp.substr(0, temp.find_last_of(","));
							}
							if (temp != "")
							{
								string t = temp;
								rx.tid = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								rx.i_count =  std::stoi(t.substr(0, t.find_first_of("_")));
								rx.type =  t.at(t.find_last_of("_") + 1);
								bs.sleep_set.push_back(rx);
								temp = "";
							}
							subs = subs.substr(subs.find_first_of("}") + 2);
							temp = subs.substr(1, subs.find_first_of(">") - 1);
	                /*Reading Relaxable events at state st in the previous run*/
							while (temp.find(",") != std::string::npos)
							{
								string t = temp.substr(temp.find_last_of(",") + 1);
								rx.tid = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								rx.i_count =  std::stoi(t.substr(0, t.find_first_of("_")));
								rx.type =  t.at(t.find_last_of("_") + 1);
								bs.relax_event.push_back(rx);
								temp = temp.substr(0, temp.find_last_of(","));
							}
							if (temp != "")
							{
								string t = temp;
								rx.tid = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								rx.i_count =  std::stoi(t.substr(0, t.find_first_of("_")));
								rx.type =  t.at(t.find_last_of("_") + 1);
								bs.relax_event.push_back(rx);
								temp = "";
							}
							subs = subs.substr(subs.find_first_of(">") + 3);
							temp = subs.substr(0, subs.length() - 1);
							while (temp.find(",") != std::string::npos)
							{
								string t = temp.substr(temp.find_last_of(",") + 1);
								ri.tid1 = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								ri.count1 =  std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(temp.find_first_of("_") + 1);
								ri.tid2 = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								ri.count2 =  std::stoi(t);
								bs.relaxed.push_back(ri);
								temp = temp.substr(0, temp.find_last_of(","));
							}
							if (temp != "")
							{
								string t = temp;
								ri.tid1 = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								ri.count1 =  std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								ri.tid2 = std::stoi(t.substr(0, t.find_first_of("_")));
								t = t.substr(t.find_first_of("_") + 1);
								ri.count2 =  std::stoi(t);
								bs.relaxed.push_back(ri);
								temp = temp.substr(0, temp.find_last_of(","));
							}
							bt_prev.push_back(bs);
						}

					}
				}
				else
				{
					first_run = true;
				}
				if (!first_run)
				{
					bool break_after = false;
					for (int i = bt_prev.size() - 1; i >= 0; i--)
					{
						bool has_slp = false;
						relax_element rx;
						rx.tid = bt_prev[i].event.tid;
						rx.i_count = bt_prev[i].event.i_count;
						for (std::deque<relax_element>::iterator rt = bt_prev[i].sleep_set.begin(); rt != bt_prev[i].sleep_set.end(); ++rt)
						{
							if ((rt->tid == rx.tid) && (rt->i_count == rx.i_count))
							{
								has_slp = true;
								break;
							}
						}
						if (!has_slp)
							bt_prev[i].sleep_set.push_back(rx);
	            /*check if it has active relax*/
						while (!bt_prev[i].relax_event.empty())
						{
							bool relax_done = false;
							race_point = i;
							relax_element rx = bt_prev[i].relax_event.front();
							bt_prev[i].relax_event.pop_front();
							tid1 = bt_prev[i].event.tid;
							tid2 = rx.tid;
							count1 = bt_prev[i].event.i_count;
							count2 = rx.i_count;
							if (tid1 != tid2)
							{
								relax_done = true;
								race_point = -1;
							}
							if ((tid1 == tid2) && (count1 > count2))
							{
								relax_done = true;
								race_point = -1;
							}
							if (!bt_prev[i].relaxed.empty())
							{
								for (std::deque<relax_info>::iterator rt = bt_prev[i].relaxed.begin(); rt != bt_prev[i].relaxed.end(); ++rt)
								{
									if ((rt->tid1 == tid2) && (rt->tid2 == tid1) && (rt->count1 == count2) && (rt->count2 == count1))
									{
										relax_done = true;
										race_point = -1;
										break;
									}
								}
							}
							for (std::deque<relax_element>::iterator rt = bt_prev[i].sleep_set.begin(); rt != bt_prev[i].sleep_set.end(); ++rt)
							{
								if ((rt->tid == tid2) && (rt->i_count == rx.i_count))
								{
									relax_done = true;
									race_point = -1;
									break;
								}
							}
							if (!relax_done)
							{
								race = false;
								ri.tid1 = bt_prev[i].event.tid;
								ri.tid2 = rx.tid;
								ri.count1 = bt_prev[i].event.i_count;
								ri.count2 = rx.i_count;
								ri.ins = "";
								relax_ds.push_back(ri);
								break_after = true;
								break;
							}
						}
						if (break_after)
							break;
	            /*checks if it has active race (not in sleep set)*/
						while (!bt_prev[i].initials.empty())
						{
							bool in_sleep = false;
							race_point = i;
							relax_element rx = bt_prev[i].initials.front();
							bt_prev[i].initials.pop_front();
							if (!bt_prev[i].sleep_set.empty())
							{
								for (int l = 0; l < bt_prev[i].sleep_set.size(); l++)
								{
									if ((bt_prev[i].sleep_set[l].tid  == rx.tid) && (bt_prev[i].sleep_set[l].i_count == rx.i_count))
									{
										race_point = -1;
										in_sleep = true;
										break;
									}
								}
							}

							if ((!in_sleep) && (bt_prev[i].event.tid != rx.tid))
							{
								race = true;
								tid1 = bt_prev[i].event.tid;
								tid2 = rx.tid;
								count1 = bt_prev[i].event.i_count;
								count2 = rx.i_count;
								break_after = true;
								break;
							}
						}
						if (break_after)
							break;
					}
					for (int i = 0; i < race_point; i++)
					{
						if (!bt_prev[i].relaxed.empty())
						{
							for (int j = 0; j < bt_prev[i].relaxed.size(); j++)
							{
								bool in_relaxds = false;
								bool in_exec = false;
								ri.tid1 = bt_prev[i].relaxed[j].tid1;
								ri.tid2 = bt_prev[i].relaxed[j].tid2;
								ri.count1 = bt_prev[i].relaxed[j].count1;
								ri.count2 = bt_prev[i].relaxed[j].count2;
	                    //bt_prev[i].relaxed.push_back(ri);
	                    for (std::deque<relax_info>::iterator rs = relax_ds.begin(); rs != relax_ds.end(); ++rs) /*Check if the pair already in relax_ds*/
								{
									if ((ri.tid1 == rs->tid1) && (ri.count1 == rs->count1) && (ri.tid2 == rs->tid2) && (ri.count2 == rs->count2))
									{
										in_relaxds = true;
										break;
									}
								}
	                    for (int l = race_point + 1; l < bt_prev.size(); l++) /*Check if the pair occurs after race_point but is recorded before*/
								{
									if ((bt_prev[l].event.tid == ri.tid2) && (bt_prev[l].event.i_count == ri.count2))
									{
										in_relaxds = true;
										break;
									}
								}
	                    for (std::deque<state>::iterator rs = exec_after.begin(); rs != exec_after.end(); ++rs) /*Check if the pair in exec_after*/
								{
									if ((ri.tid1 == rs->tid) && (ri.count1 == rs->count))
									{
										in_exec = true;
										break;
									}
								}
								if ((!in_relaxds) && (in_exec))
								{
	                        //cout << " add relax "<< ri.tid1 << " " << ri.count1 << " "<< ri.tid2 << " " << ri.count2<<endl;
				bool widen_relax = false; // update the relaxable pairs
				for (std::deque<relax_info>::iterator it = relax_ds.begin(); it != relax_ds.end(); ++it)
				{
					if ((it->tid1 == ri.tid1) && (it->count1 == ri.count1))
					{
						it->count2 = it->count2 > ri.count2 ? it->count2 : ri.count2; 
						widen_relax = true;
						break;
					}
					if ((it->tid1 == ri.tid1) && (it->count1 == ri.count2))
					{
						it->count1 = ri.count1; 
						widen_relax = true;
						break;
					}
					if ((it->tid1 == ri.tid1) && (it->count2 == ri.count1))
					{
						it->count2 = ri.count2; 
						widen_relax = true;
						break;
					}
				}
				if (!widen_relax)
				{
					relax_ds.push_back(ri);
					cout << "ADDING RELAX " << ri.tid1 << " " << ri.count1 << " " << ri.tid2 << ri.count2 << endl;
				}
			}
		}
	}
}
if (race_point < 0)
{
	endrun.open("endrun.out");
	endrun << "true" << endl;
	endrun.close();
	endrun_set = true;
	cout << "Runs Exhausted" << endl;
	exit(0);
}
else
{
	racepoint_sleep = bt_prev[race_point].sleep_set;
	sleep_element se;
	for (std::deque<relax_element>::iterator it = racepoint_sleep.begin(); it != racepoint_sleep.end(); ++it)
	{
		se.tid = it->tid;
		se.count = it->i_count;
		race_sleep.push_back(se);
	}
	if (race_point == 0)
	{
		break_point.tid = 0;
		break_point.count = 1;
	}
	else
	{
		break_point.tid = bt_prev[race_point - 1].event.tid;
		break_point.count = bt_prev[race_point - 1].event.i_count;
	}
	endrun.open("endrun.out");
	endrun << "race"  << endl;
	endrun.close();
}
if (race_point >= 0)
{
	            //for (std::deque<state>::iterator rt = exec_after.begin(); rt != exec_after.end(); ++rt)
	while (!exec_after.empty())
	{
		if (exec_after.front().index <= race_point+1)
		{
	                    //if ((exec_after.front().tid != tid1) && (exec_after.front().count != count1))
			{
				cout << "Exect POP : " <<  exec_after.front().tid << " " << exec_after.front().count << endl;
				exec_after.pop_front();
			}
		}
		else
		{
			break;
		}
	}
}
}
if (first_run)
{
	break_point.tid = -100;
	break_point.count = -100;
}
	    /* if (race)
	     {
	         if ((break_point.tid == tid1) && (break_point.count > count1))
	             break_point.count = count1;
	         if ((break_point.tid == tid2) && (break_point.count > count2))
	             break_point.count = count2;
	     }*/
if (!race)
{
	int up = 1;
	while ((break_point.tid == tid1) && (break_point.count > count1))
	{
	            //break_point.count = count1 - 1;
		up++;
		break_point.tid = bt_prev[race_point - up].event.tid;
		break_point.count = bt_prev[race_point - up].event.i_count;
		cout << "New breakpoint " << break_point.tid << " " << break_point.count << endl;
	}
}
second = std::to_string(tid2) + "_" + std::to_string(count2);
cout << "PIN: " << tid1 << " " << count1 << " " << tid2 << " " << count2 << " " << break_point.tid << " " << break_point.count <<" "<< race<< endl;
stack_size = stack.size();

cout << "PIN:  start " << race_point << endl;
stack.pop_front();
if (stack.size() > 2) {
	curr_state = stack.front();
	stack.pop_front();
	next_state = stack.front();
}
races.open("races.out");
allLocks.reserve(20);

PIN_InitSymbols();
if ( PIN_Init(argc, argv) )
{
	return Usage();
}


	    // pinplay_engine.Activate(argc, argv,
	    // KnobPinPlayLogger, KnobPinPlayReplayer);
start_s = clock();
tls_key = PIN_CreateThreadDataKey(0);
PIN_InitLock(&GlobalLock);

PIN_AddThreadStartFunction(ThreadStart, 0);
PIN_AddThreadFiniFunction(ThreadFini, 0);
IMG_AddInstrumentFunction(Image, 0);
TRACE_AddInstrumentFunction(Trace, 0);

PIN_AddFiniFunction(Fini, 0);

filter.Activate();

PIN_StartProgram();
return 0;
}
