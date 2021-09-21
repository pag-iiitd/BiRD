#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

#include <stdio.h>
#include <stdint.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <list>
#include <ostream>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <cmath>
#include <set>
#include "pin.H"
#include "instlib.H"

using namespace INSTLIB;
using namespace std;

#define CMPXCHG 111
#define EAX_REG 10
#define RFLAGS_REG 25

typedef struct MemoryWrite_t {
    ADDRINT effective_address;
    UINT32 size;
} MemoryWrite;

#endif