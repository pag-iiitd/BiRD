#include "ThreadLocalBuffer.h"

extern TLS_KEY tls_key;

ThreadLocalBuffer::ThreadLocalBuffer(THREADID tid)
{

}

ThreadLocalBuffer* getTLB(THREADID tid)
{
	ThreadLocalBuffer* tld = static_cast<ThreadLocalBuffer*>(PIN_GetThreadData(tls_key,tid));
	return tld;
}

