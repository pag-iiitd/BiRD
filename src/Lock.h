#ifndef LOCK_H_
#define LOCK_H_

#include "pin.H"
#include "VectorClock.h"

class Lock{
	public:
		Lock(ADDRINT addrIn);

		VectorClock* lockVectorClock;

		vector<VectorClock> recordClocks;
		vector< set<ADDRINT> > atomicReadRegions;
		vector< set<ADDRINT> > atomicWriteRegions;

		set<ADDRINT> memReadAccesses;
		set<ADDRINT> memWriteAccesses;

		int lock_inst;
		int unlock_inst;

		PIN_LOCK LockProtect;

		ADDRINT addr;
		bool isLocked;
		void lock_it(int lock_inst);
		void unlock_it(int unlock_inst);
};

struct lock_has_addr
{
    lock_has_addr(ADDRINT const& n) : addr(n) { }  
    bool operator () (Lock const* p) { return p->addr == addr; }
private:
    ADDRINT addr;
};

#endif