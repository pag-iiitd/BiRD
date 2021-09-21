#ifndef THREADLOCALDATA_H_
#define THREADLOCALDATA_H_

#include "definitions.h"
#include "VectorClock.h"
#include "Lock.h"

class ThreadLocalData
{
	public:
		ThreadLocalData(THREADID tid);
		
		VectorClock* currentVectorClock;
		ofstream out;
		ofstream thread_trace;
		std::queue<MemoryWrite *> memoryOperandsBefore;
		std::queue<MemoryWrite *> memoryOperandsAfter;
		int isEAXZero;
		int isEAXTwo;
		// 0 states
		int isZeroBefore;
		int isZeroAfter;
		// 1 states
		int isOneAfterAndLocked;
		int isOneAfterAndUnlocked;
		int isOneBeforeAndLocked;
		int isOneBeforeAndUnlocked;
		// 2 states
		int isTwoBefore;
		int isTwoAfter;

		int isCMPXCHG;
		THREADID threadId;
		THREADID parentThreadId;
		int insCount;
		int insCount2;
		bool blockBegin;
		int begin;
		bool blockSet;
		bool blockReset;
		int block_prevSize;
		int block_start;
		int block_size;

		PIN_LOCK threadLock;

		void resetVars();
		void addAddressToLockWrite(ADDRINT addr);
		void addAddressToLockRead(ADDRINT addr);

		set<ADDRINT> readSet;
		set<ADDRINT> writeSet;

		list<Lock *> acqLocks;
};

ThreadLocalData* getTLS(THREADID tid);


#endif
