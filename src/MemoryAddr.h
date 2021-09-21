#ifndef MEMORYADDR_H_
#define MEMORYADDR_H_

#include "definitions.h"
#include "VectorClock.h"

class MemoryAddr
{
public:
	MemoryAddr(ADDRINT addr);
	ADDRINT addr;

	PIN_LOCK MemoryLock;
        
        vector<int> operand_index;
	vector<char> accesses;
	vector<THREADID> accessingThread;
	vector<int> accessingInstructions;
	vector<VectorClock> accessClocks;
};

struct mem_has_addr
{
    mem_has_addr(ADDRINT const& n) : addr(n) { }  
    bool operator () (MemoryAddr const* p) { return p->addr == addr; }
private:
    ADDRINT addr;
};

#endif /* MEMORYADDR_H_ */
