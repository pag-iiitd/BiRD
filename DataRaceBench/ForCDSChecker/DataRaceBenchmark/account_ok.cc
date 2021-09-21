#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <stdatomic.h>
#include <threads.h>
#include "librace.h"
#include "model-assert.h"

std::mutex m;
atomic_int y;
atomic_int z;
atomic_int x;
atomic_int balance;
atomic_bool deposit_done;
atomic_bool withdraw_done;

void *deposit(void *arg) 
{

  //pthread_mutex_lock(&m);
	m.lock();
  //balance = balance + y;
  //deposit_done=1;
  int y1=atomic_load_explicit(&y , memory_order_relaxed);	
	atomic_fetch_add_explicit(&balance, y1, memory_order_relaxed);
	atomic_store_explicit(&deposit_done, 1, memory_order_relaxed);
  //pthread_mutex_unlock(&m);
	m.unlock();

}

void *withdraw(void *arg) 
{
  //pthread_mutex_lock(&m);
	m.lock();
  //balance = balance - z;
  //withdraw_done=1;
	int z1=atomic_load_explicit(&z, memory_order_relaxed);
	atomic_fetch_sub_explicit(&balance, z1, memory_order_relaxed);
	atomic_store_explicit(&withdraw_done, 1, memory_order_relaxed);
  //pthread_mutex_unlock(&m);
	m.unlock();
}

void *check_result(void *arg) 
{

 //pthread_mutex_lock(&m);
	m.lock();
	int r1=atomic_load_explicit(&deposit_done , memory_order_relaxed);
	int r2=atomic_load_explicit(&withdraw_done, memory_order_relaxed);
  //if (deposit_done && withdraw_done)
	if (r1 && r2)
	{
	 	int b1=atomic_load_explicit(&balance , memory_order_relaxed);	
		int x2=atomic_load_explicit(&x , memory_order_relaxed);	
		int y2=atomic_load_explicit(&y , memory_order_relaxed);	
		int z2=atomic_load_explicit(&z , memory_order_relaxed);	
    //MODEL_ASSERT(balance == (x + y) - z); /* BAD */
    MODEL_ASSERT(b1 == (x2 + y2) - z2); /* BAD */
	}
 // pthread_mutex_unlock(&m);
	m.unlock();
}

int user_main(int argc, char **argv)
{
 	atomic_store_explicit(&deposit_done, 0, memory_order_relaxed);
	atomic_store_explicit(&withdraw_done, 0, memory_order_relaxed);
  thrd_t t1, t2, t3;

 // pthread_mutex_init(&m, 0);
	atomic_store_explicit(&x, 1, memory_order_relaxed);
	atomic_store_explicit(&y, 2, memory_order_relaxed);
	atomic_store_explicit(&z, 4, memory_order_relaxed);
	int x1=atomic_load_explicit(&x, memory_order_relaxed);
	atomic_store_explicit(&balance, x1, memory_order_relaxed);
  //x = 1;
  //y = 2;
  //z = 4;
  //balance = x;

	thrd_create(&t3, (thrd_start_t)check_result, NULL);
	thrd_create(&t1, (thrd_start_t)deposit, NULL);
	thrd_create(&t2, (thrd_start_t)withdraw, NULL);
  //pthread_create(&t3, 0, check_result, 0);
  //pthread_create(&t1, 0, deposit, 0);
  //pthread_create(&t2, 0, withdraw, 0);

  return 0;
}

