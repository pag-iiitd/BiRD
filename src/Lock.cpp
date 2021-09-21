#include "Lock.h"

Lock::Lock(ADDRINT addrIn)
{
	addr = addrIn;
	lockVectorClock = new VectorClock(-1, 100);

	isLocked = false;
	PIN_InitLock(&LockProtect);
}

void Lock::lock_it(int lock_inst)
{
	isLocked = true;
	this->lock_inst = lock_inst;
}

void Lock::unlock_it(int unlock_inst)
{
	isLocked = false;
	this->unlock_inst = unlock_inst;
}