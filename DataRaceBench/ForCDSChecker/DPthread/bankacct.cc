/**
 * A classical bank account example that demonstrate atomicity violation. 
 *
 * Author: Heechul Yun <heechul@illinois.edu> 
 *
 * This f#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <stdatomic.h>
#include <threads.h>
#include "librace.h"
#include "model-assert.h"ile is distributed under the University of Illinois Open Source
 * License. See LICENSE.TXT for details.
 *
 */ 
#include <stdio.h>
#include <sys/mman.h>
#include <string.h>
#include <err.h>
#include <unistd.h>
#include <pthread.h>
#include <unistd.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <stdatomic.h>
#include <threads.h>
#include "librace.h"
#include "model-assert.h"
// NOTE: uncomment the following line to make app deterministic 
// #include <dpthread-wrapper.h>

// defines 
#define DBG(x) x 

// glocal variables 
atomic_int g_account;  // account 

//static pthread_mutex_t lock; 

std::mutex m;
// worker definitions 
void *Deposit(int amount)
{
	int balance; // ERROR: balance can be stale

	//pthread_mutex_lock(&lock); 
	m.lock();
	balance = atomic_load_explicit(&g_account , memory_order_relaxed);	//g_account; 
	//pthread_mutex_unlock(&lock); 
	m.unlock();
	
	//pthread_mutex_lock(&lock);
	m.lock();
	int total = balance + amount;
	atomic_store_explicit(&g_account, total, memory_order_relaxed);
	//g_account = balance + amount; 
	//pthread_mutex_unlock(&lock); 
	m.unlock();

	return NULL; 
}

void *Withdraw(int amount)
{
	int balance; // ERROR: balance can be stale

	m.lock();
	//pthread_mutex_lock(&lock);
	balance = atomic_load_explicit(&g_account , memory_order_relaxed);//g_account; 
	//pthread_mutex_unlock(&lock); 
	m.unlock();
	
	//pthread_mutex_lock(&lock);
	m.lock();
	int total = balance - amount;
	atomic_store_explicit(&g_account, total, memory_order_relaxed);
	//g_account = balance - amount; 
	//pthread_mutex_unlock(&lock); 
	m.unlock();
	return NULL; 
}


// main 
static void
usage(char *argv[])
{
	printf("%s -x <x> -y <y>\n"
	       "-x : input for thread 1\n" 
	       "-y : input for thread 2\n", argv[0]); 
}

int user_main(int argc, char **argv)
//int main(int argc, char *argv[])
{
	thrd_t allthr[10];
	int i, ret; 
	int x, y; 

	// initialize 
	x = 13; 
	y = 0; 

	// parameters 
	while((i=getopt(argc, argv, "x:y:h")) != EOF) {
		switch(i) {
		case 'h':
			usage(argv);
			return 0;
		case 'x':
			x = strtol(optarg, NULL, 0); 
			break; 
		case 'y': 
			y = strtol(optarg, NULL, 0); 
			break; 
		default:
			errx(1, "invalid option");
		}
	}

	// lock initialize 
	//pthread_mutex_init(&lock, NULL);

	//g_account = 10; 
	atomic_store_explicit(&g_account, 10, memory_order_relaxed);

	// thread create 
	ret = thrd_create(&allthr[0], (thrd_start_t)Deposit, (void *)1);
	if (ret) err(1, "pthread_create failed");
	Withdraw(2); 

	// wait for workers to finish. 
	thrd_join(allthr[0]);
	
	int g_a = atomic_load_explicit(&g_account , memory_order_relaxed);
	assert( g_a == 9 ); 

	return 0; 
}
