#include "VectorClock.h"


ostream& operator<<(ostream& os, const VectorClock& v)
{
	os << "Vector Clock Of " << v.threadId << ":" << endl;
	UINT32 *values = v.getValues();
	for (int i = 0; i < v.totalThreadCount; ++i)
	{
		os << setw(33) << values[i];
		if ( (i +1) % 4 == 0)
			os << endl;
	}

	return os;
}

VectorClock& VectorClock::operator++()
{
	vclock_arr[threadId]++;
	return *this;
}

VectorClock VectorClock::operator++(int)
{
	VectorClock tmp = *this;
	vclock_arr[threadId]++;
	return tmp;
}

bool VectorClock::operator<=(const VectorClock& vRight)
{
	if (operator<(vRight)  || operator==(vRight) )
		return true;
	return false;
}

bool VectorClock::operator==(const VectorClock& vRight)
{
	UINT32 *vRightValues = vRight.getValues();
	for (int i = 0; i < totalThreadCount; ++i)
		if (vclock_arr[i] != vRightValues[i])
			return false;

		return true;
}

bool VectorClock::operator!=(const VectorClock& vRight)
{
	return !(operator==(vRight));
}

bool VectorClock::operator<(const VectorClock& vRight)
{
	bool strictlySmaller = false;
	UINT32* vRightValues = vRight.getValues();
	for (int i = 0; i < totalThreadCount; ++i)
	{
		if ( vclock_arr[i] <= vRightValues[i] )		//at least ONE value is stricly smaller
			strictlySmaller = true;
		else if (vclock_arr[i] > vRightValues[i])  	//if any value of v[i] is greater, than no way v<vRight
			return false;//return false;
	}

	return strictlySmaller;
}

bool VectorClock::happensBefore(VectorClock* input)
{
	    bool happensBeforeValue = (*this < *input);
		return  happensBeforeValue;
}

bool VectorClock::areConcurrent(VectorClock* input)
{
	if  (!this->happensBefore(input) &&  !input->happensBefore(this))
		return true;

	return false;
}

VectorClock::VectorClock(int threadIDIn, int totalthreads)
{
	threadId = threadIDIn;
	totalThreadCount = totalthreads;

	size_t size_of_bytes = sizeof(UINT32) * totalThreadCount;
	vclock_arr = (UINT32*)malloc(size_of_bytes);
	bzero(vclock_arr, size_of_bytes);
}

VectorClock::VectorClock(VectorClock* inClockPtr, int threadIdIn)
{
	threadId = threadIdIn;
	totalThreadCount = inClockPtr->totalThreadCount;
	size_t size_of_bytes = sizeof(int) * totalThreadCount;
	vclock_arr = (UINT32*)malloc(size_of_bytes);
	bzero(vclock_arr, size_of_bytes); //bzero yapmak onemli
	for (int i = 0; i < totalThreadCount; ++i )
		vclock_arr[i] = inClockPtr->vclock_arr[i];
}

VectorClock::VectorClock(const VectorClock& old_clock){
	threadId = old_clock.threadId;
	totalThreadCount = old_clock.totalThreadCount;
	size_t size_of_bytes = sizeof(UINT32) * totalThreadCount;
	vclock_arr = (UINT32*)malloc(size_of_bytes);
	memcpy(vclock_arr, old_clock.vclock_arr, size_of_bytes);
}

VectorClock::~VectorClock()
{
	free(vclock_arr);
}

void VectorClock::sendEvent()
{
	event();
}

void VectorClock::event()
{
	vclock_arr[threadId]++;
}

void VectorClock::incEvent(THREADID tid)
{
	vclock_arr[tid]++;
}

UINT32* VectorClock::getValues() const
{
	return vclock_arr;
}



void VectorClock::receiveAction(VectorClock* vectorClockReceived)
{
	UINT32 *vOfReceivedClock = vectorClockReceived->getValues();
	for (int i = 0; i < totalThreadCount; ++i)
		vclock_arr[i] = ( vclock_arr[i] > vOfReceivedClock[i] ) ? vclock_arr[i] : vOfReceivedClock[i];
}

void VectorClock::receiveActionFromParent(VectorClock* vectorClockReceived, int parent)
{
	UINT32 *vOfReceivedClock = vectorClockReceived->getValues();
	for (int i = 0; i < totalThreadCount; ++i){
		if((i==parent) )
		         vclock_arr[i] = ( vclock_arr[i] > vOfReceivedClock[i] ) ? vclock_arr[i] : vOfReceivedClock[i];
		else
			continue;
	}

}

void VectorClock::receiveActionFromSpecialPoint(VectorClock* vectorClockReceived, int specialPoint)
{
	UINT32 *vOfReceivedClock = vectorClockReceived->getValues();
	for (int i = 0; i < totalThreadCount; ++i){
		if(i==specialPoint)
			continue;
		vclock_arr[i] = ( vclock_arr[i] > vOfReceivedClock[i] ) ? vclock_arr[i] : vOfReceivedClock[i];
	}

}

bool VectorClock::isUniqueValue(int threadid)
{
	bool isUnique = true;
	for (int i = 0; i < totalThreadCount ; ++i)
	{
		if (vclock_arr[i] > 0 && i != threadid)
		{
			isUnique = false;;
			break;
		}
	}

	return isUnique;
}
