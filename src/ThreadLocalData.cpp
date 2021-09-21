#include "ThreadLocalData.h"

extern TLS_KEY tls_key;

ThreadLocalData::ThreadLocalData(THREADID tid)
{
	currentVectorClock = new VectorClock(tid, 100);
    isEAXZero = 0;
    isEAXTwo = 0;
	// 0 states
	isZeroBefore = 0;
	isZeroAfter = 0;
	// 1 states
	isOneAfterAndLocked = 0;
	isOneAfterAndUnlocked = 0;
	isOneBeforeAndLocked = 0;
	isOneBeforeAndUnlocked = 0;
	// 2 states
	isTwoBefore = 0;
	isTwoAfter = 0;
	insCount = 0;
	insCount2 = 0;
	block_start = 0;
	block_size = 0;
	block_prevSize = 0;
	isCMPXCHG = 0;
	blockBegin = false;
	begin = 0;
	blockSet = false;
	blockReset = false;
	threadId = tid;
	parentThreadId = -1;

	PIN_InitLock(&threadLock);
}

void ThreadLocalData::resetVars(){
	isEAXZero = 0;
        isEAXTwo = 0;
	// 0 states
	isZeroBefore = 0;
	isZeroAfter = 0;
	// 1 states
	isOneAfterAndLocked = 0;
	isOneAfterAndUnlocked = 0;
	isOneBeforeAndLocked = 0;
	isOneBeforeAndUnlocked = 0;
	// 2 states
	isTwoBefore = 0;
	isTwoAfter = 0;
	
	isCMPXCHG = 0;
}

ThreadLocalData* getTLS(THREADID tid)
{
	ThreadLocalData* tld = static_cast<ThreadLocalData*>(PIN_GetThreadData(tls_key,tid));
	return tld;
}

void ThreadLocalData::addAddressToLockWrite(ADDRINT addr){
	list<Lock*>::const_iterator pos;
	for(pos=acqLocks.begin(); pos!=acqLocks.end(); ++pos){
		PIN_GetLock(&((*pos)->LockProtect), threadId);
		(*pos)->memWriteAccesses.insert(addr);
		PIN_ReleaseLock(&((*pos)->LockProtect));
	}
}

void ThreadLocalData::addAddressToLockRead(ADDRINT addr){
	list<Lock*>::const_iterator pos;
	for(pos=acqLocks.begin(); pos!=acqLocks.end(); ++pos){
		PIN_GetLock(&((*pos)->LockProtect), threadId);
		(*pos)->memReadAccesses.insert(addr);
		PIN_ReleaseLock(&((*pos)->LockProtect));
	}
}
