#ifndef CONTROL_H_
#define CONTROL_H_

#include "VectorClock.h"
#include <semaphore.h>


struct relax_element
    {
    THREADID tid;
    char type;
    string ins = "";
    int i_count;
    ADDRINT addr;
    VectorClock* vc;
    bool islock = false;
    int value = -999;
    bool present = false;
    bool exhaust = false;
    };


struct relax_info
{
  int tid1;
  int tid2;
  int count1;
  int count2;
  ADDRINT addr1;
  ADDRINT addr2;
  string ins = "";
  CONTEXT * ctxt1 = new CONTEXT;
  CONTEXT * ctxt2 = new CONTEXT;
  bool executed1 = false;
  bool executed2 = false;
    ADDRINT ins_addr;
    ADDRINT memOp;
    ADDRINT prev_value;
    ADDRINT value;

  bool done = false;
  bool pro = false;
  bool incremented = false;
  bool in_race_sleep = false;
};

struct state
{
    int tid;
    int count;
    bool done = false;
    bool pro = false;
    int index = 0;
};

struct stack_element
{
int count;
bool enabled = false;
int count2 = 0;
};

struct bt_state
{
relax_element event;
deque<relax_element> initials = {};
deque<relax_element> sleep_set = {};
deque<relax_element> enabled_set = {};
deque<relax_element> relax_event = {};
deque<relax_info> relaxed = {};
};

struct fence_element
{
    int tid;
    int count;
};

struct sleep_element
{
    THREADID tid;
    char type;
    string ins;
    int count;
    ADDRINT addr;
    VectorClock* vc;
    bool islock = false;
    bool present = false; 
    bool exhaust = false;
};

struct threadInfo
{
    THREADID parent;
    THREADID tid;
    ADDRINT init_addr = NULL;
    int start = 0;
    int end = 0;
    ADDRINT regAddr;
    bool joined = false;
};

/*struct createJoin
{
    THREADID tid;
    int create = 0;
    int join = 0;
    ADDRINT reg = NULL;
    bool joined = false;
};*/

struct startInfo
{
    THREADID tid;
    ADDRINT start_addr;
    int start_count = 0;
};

struct lockedRegion
{
    THREADID tid;
    int start = 0;
    int end = 0;
};
struct lockInfo
{
ADDRINT addr;
deque<lockedRegion> locked_region;
};

struct writeRelax
{
    bool done =false;
    THREADID tid;
    int i_count1 = 0;
    int i_count2 = 0;
    ADDRINT ins_addr;
    ADDRINT memOp;
    ADDRINT prev_value;
    ADDRINT value;
    bool executed1 = false;
    bool executed2 = false;
};
#endif
