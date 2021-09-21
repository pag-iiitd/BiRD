#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <stdatomic.h>
#include <threads.h>
#include "librace.h"
#include "model-assert.h"

atomic_int times;
atomic_uint k;


void * thread(void *)
{
		int t=atomic_load_explicit(&times , memory_order_relaxed);	
    for(unsigned i=0;i<t;++i)
        atomic_store_explicit(&k, 0, memory_order_relaxed);
    return NULL;
}

bool main_thread()
{
    int i=0,j,k1;
    int t=atomic_load_explicit(&times , memory_order_relaxed);	
    for(j=0;j<t;++j)
    {
    		k1 = atomic_load_explicit(&k , memory_order_relaxed);
        if(k1==0)
            ++i;
        atomic_store_explicit(&k, 1, memory_order_relaxed);
    }
    int t1 = atomic_load_explicit(&times , memory_order_relaxed);
    return i<t1;
}


int user_main(int argc, char **argv)
{
		atomic_store_explicit(&k, 0, memory_order_relaxed);	
		atomic_store_explicit(&times, 2, memory_order_relaxed);	
    thrd_t pid;

    assert(thrd_create(&pid,(thrd_start_t)thread,NULL) == 0);
    
    bool res = main_thread();
    
    thrd_join(pid);
    
    assert(res);
    return 0;
}

