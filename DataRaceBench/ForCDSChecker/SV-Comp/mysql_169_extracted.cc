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

#include <vector>
#include <map>
//#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cstdio>
//#include <cstdlib>
#include <stdlib.h>
#include <assert.h>
#include <mutex>
#include <stdatomic.h>
#include <threads.h>
#include "librace.h"
#include "model-assert.h"


class MYSQL_LOG {
 public:
  MYSQL_LOG() {
    //pthread_mutex_init(&mutex_, NULL);
  }

  ~MYSQL_LOG() {}

  void write(const char *content) {
    mutex_.lock();
    contents_.push_back(content);
    mutex_.unlock();
  }

  const char *get_content(int i) {
    return contents_[i];
  }

 private:
  void Lock() { 
  	mutex_.lock();
  //pthread_mutex_lock(&mutex_); 
  }
  void Unlock() { 
    mutex_.unlock();
  //pthread_mutex_unlock(&mutex_); 
  }
	std::mutex mutex_;
  //pthread_mutex_t mutex_;
  //std::vector<const char *> contents_;
	std::vector<std::atomic<char *>> contents_;
};

class MYSQL_TABLE {
 public:
  MYSQL_TABLE() {
    pthread_mutex_init(&mutex_, NULL);
  }

  ~MYSQL_TABLE() {}

  void insert_entry(int key, int val) {
    mutex_.lock();
    contents_[key] = val;
    mutex_.unlock();
  }

  void remove_entries() {
    mutex_.lock();
    contents_.clear();
    mutex_.unlock();
  }

  bool empty() {
    return contents_.empty();
  }

 private:
  void Lock() { 
  	mutex_.lock();
  //pthread_mutex_lock(&mutex_); 
  }
  void Unlock() {
  	mutex_.unlock();
  //pthread_mutex_unlock(&mutex_); 
  }
	std::mutex_t mutex_;
 // pthread_mutex_t mutex_;
  std::map<int, int> contents_;
};

MYSQL_LOG mysql_log;
MYSQL_TABLE table;

void *delete_thread_main(void *args) {
  printf("removing\n");
  table.remove_entries();
  mysql_log.write("remove");
  printf("removing done\n");
  return NULL;
}

void *insert_thread_main(void *args) {
  printf("inserting\n");
  table.insert_entry(1, 2);
  mysql_log.write("insert");
  printf("inserting done\n");
  return NULL;
}

int user_main(int argc, char **argv)  {
  thrd_t delete_tid;
  thrd_t insert_tid;
  thrd_create(&delete_tid, (thrd_start_t)delete_thread_main, NULL);
  thrd_create(&insert_tid, (thrd_start_t)insert_thread_main, NULL);
  thrd_join(delete_tid);
  thrd_join(insert_tid);

  // validate results
  if (table.empty()) {
    assert(!strcmp(mysql_log.get_content(0), "insert"));
  } else {
    assert(!strcmp(mysql_log.get_content(0), "remove"));
  }

  printf("Program exit normally\n");

  return 0;
}

