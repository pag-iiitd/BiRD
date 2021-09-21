#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <stdatomic.h>
#include <threads.h>
#include "librace.h"
#include "model-assert.h"
//#include "common.inc"

#define TRUE  (1)
#define FALSE (0)

std:: mutex m;
//void __ESBMC_atomic_begin();
//void __ESBMC_atomic_end();

typedef struct DEVICE {
  atomic_int pendingIo;
  atomic_bool stoppingFlag;
  atomic_bool stoppingEvent;
} DEVICE_EXTENSION;

atomic_bool stopped;

int BCSP_IoIncrement(DEVICE_EXTENSION *e)
{
	bool e_s = atomic_load_explicit(&(e->stoppingFlag) , memory_order_relaxed);
  if (e_s)
    return -1;
	m.lock();
  //__ESBMC_atomic_begin();
  atomic_fetch_add_explicit(&(e->pendingIo), 1, memory_order_relaxed);
  //e->pendingIo = e->pendingIo + 1;
  //__ESBMC_atomic_end();  
  m.unlock();
  return 0;
}

void BCSP_IoDecrement(DEVICE_EXTENSION *e)
{
  int pendingIo;
	m.lock();
  //__ESBMC_atomic_begin();
    //atomic_store_explicit(&(account->balance), balance, memory_order_relaxed);
    atomic_fetch_sub_explicit(&(e->pendingIo), 1, memory_order_relaxed);
    pendingIo = atomic_load_explicit(&(e->pendingIo) , memory_order_relaxed);
  //e->pendingIo = e->pendingIo - 1;
  //pendingIo = e->pendingIo;
  //__ESBMC_atomic_end();  
	m.unlock();
  if (pendingIo == 0)
  	atomic_store_explicit(&(e->stoppingEvent), TRUE, memory_order_relaxed);
    //e->stoppingEvent = TRUE;
}

void BCSP_PnpAdd(DEVICE_EXTENSION *e)
{
  int status;

  status = BCSP_IoIncrement(e);
  if (status == 0)
  {
    //do work here
    bool s = atomic_load_explicit(&stopped , memory_order_relaxed);
    assert(!s);
  }
  BCSP_IoDecrement(e);
}

void* BCSP_PnpStop(void* arg)
{
  DEVICE_EXTENSION *e;
  e = (DEVICE_EXTENSION *) arg; 

	atomic_store_explicit(&(e->stoppingFlag), TRUE, memory_order_relaxed);
  //e->stoppingFlag = TRUE;
  BCSP_IoDecrement(e);
  bool e_s = atomic_load_explicit(&(e->stoppingEvent) , memory_order_relaxed);
  if(e_s)
  {
    //release allocated resource
    //stopped = TRUE;
    atomic_store_explicit(&stopped, TRUE, memory_order_relaxed);
  }
}

int user_main(int argc, char **argv) 
{
  thrd_t id;
  DEVICE_EXTENSION e;
	
	atomic_store_explicit(&(e.pendingIo), 1, memory_order_relaxed);
	atomic_store_explicit(&(e.stoppingFlag), FALSE, memory_order_relaxed);
	atomic_store_explicit(&(e.stoppingEvent), FALSE, memory_order_relaxed);
	atomic_store_explicit(&stopped, FALSE, memory_order_relaxed);
  //e.pendingIo = 1;
  //e.stoppingFlag = FALSE;
  //e.stoppingEvent = FALSE;
  //stopped = FALSE;

  thrd_create(&id, (thrd_start_t) BCSP_PnpStop, (void*) &e);
  BCSP_PnpAdd(&e);
  thrd_join(id);
}
