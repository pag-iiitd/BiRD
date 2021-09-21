#ifndef THREADLOCALBUFFER_H_
#define THREADLOCALBUFFER_H_

#include "definitions.h"
#include "VectorClock.h"
#include "Lock.h"

class ThreadLocalBuffer
{
	public:
		ThreadLocalBuffer(THREADID tid);

		struct entity
			{
			THREADID tid;
			int i_count;
			string ins;
			ADDRINT addr;
			VectorClock* vc;
			};
		vector<entity> storebuffer;

              
};

ThreadLocalBuffer* getTLB(THREADID tid);


#endif
