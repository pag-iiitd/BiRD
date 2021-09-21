#include "MemoryAddr.h"


MemoryAddr::MemoryAddr(ADDRINT addr)
{
	this->addr = addr;
	PIN_InitLock(&MemoryLock);
}