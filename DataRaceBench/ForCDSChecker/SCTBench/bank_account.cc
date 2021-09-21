// Copyright 2011 The University of Michigan
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors - Jie Yu (jieyu@umich.edu)

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

// typedefs
typedef struct {
  atomic_int balance;
  //pthread_mutex_t lock;
  std::mutex lock; 
} bank_account_type;

int get_balance(bank_account_type *account) {
  int ret = 0;
  account->lock.lock();
  //pthread_mutex_lock(&account->lock);
  ret = atomic_load_explicit(&(account->balance) , memory_order_relaxed);//account->balance;
  //pthread_mutex_unlock(&account->lock);
  account->lock.unlock();
  return ret;
}

void set_balance(bank_account_type *account, int balance) {
  //pthread_mutex_lock(&account->lock);
  account->lock.lock();
  //account->balance = balance;
  atomic_store_explicit(&(account->balance), balance, memory_order_relaxed);
  //pthread_mutex_unlock(&account->lock);
  account->lock.unlock();
}

void withdraw(bank_account_type *account, int amount) {
  int curr_balance = 0;
  curr_balance = get_balance(account);
  curr_balance -= amount;
  set_balance(account, curr_balance);
}

void deposit(bank_account_type *account, int amount) {
  int curr_balance = 0;
  curr_balance = get_balance(account);
  curr_balance += amount;
  set_balance(account, curr_balance);
}

void init_account(bank_account_type *account) {
  //account->balance = 0;
  atomic_store_explicit(&(account->balance), 0, memory_order_relaxed);
  //pthread_mutex_init(&account->lock, NULL);
}

static int amount = 20;

void *t1_main(void *args) {
  bank_account_type *account = (bank_account_type *)args;
  printf("t1 is depositing %d\n", amount);
  deposit(account, amount);
  printf("deposit done\n");
  return NULL;
}

void *t2_main(void *args) {
  bank_account_type *account = (bank_account_type *)args;
  printf("t1 is withdrawing %d\n", amount);
  withdraw(account, 20);
  printf("withdraw done\n");
  return NULL;
}

int user_main(int argc, char **argv) {
  // allocate bank account
  bank_account_type *account = new bank_account_type;
  init_account(account);

  thrd_t *tids = new thrd_t[2];
  thrd_create(&tids[0],  (thrd_start_t)t1_main, (void *)account);
  thrd_create(&tids[1],  (thrd_start_t)t2_main, (void *)account);
  thrd_join(tids[0]);
  thrd_join(tids[1]);

  // print results
  int r1 = atomic_load_explicit(&(account->balance) , memory_order_relaxed);
  printf("balance = %d\n", r1);
  assert(r1 == 0);

  return 0;
}
