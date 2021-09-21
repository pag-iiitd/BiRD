#ifndef VECTORCLOCK_H_
#define VECTORCLOCK_H_

#include "definitions.h"

class VectorClock{
	public:
		UINT32* vclock_arr;
	public:
		int threadId;
		int totalThreadCount;
		void event();
		void incEvent(THREADID threadid);
		VectorClock(int threadIDIn, int totalthreads);
		~VectorClock();
		VectorClock(VectorClock* inClockPtr, int threadIDIn);
		VectorClock(const VectorClock&);
		void receiveAction(VectorClock* vectorClockReceived);
		void receiveActionFromSpecialPoint(VectorClock* vectorClockReceived, int specialPoint);
		void receiveActionFromParent(VectorClock* vectorClockReceived, int parent);
		void sendEvent();
		bool happensBefore(VectorClock* input);
		bool isUniqueValue(int threadid);
		UINT32* getValues() const;
		VectorClock& operator++();
		VectorClock operator++(int x);
		bool operator==(const VectorClock &vRight);
		bool operator!=(const VectorClock &vRight);
		bool operator<(const VectorClock& vRight);
		bool operator<=(const VectorClock& vRight);
		bool areConcurrent(VectorClock* vectorClockReceived);
		friend ostream& operator<<(ostream& os, const VectorClock &v);
};

#endif
