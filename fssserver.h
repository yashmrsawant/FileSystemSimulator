#ifndef FSSSERVER_H_
#define FSSSERVER_H_

#include "fssinit.h"

FILE* fss_area;
LLT blockNumber;
LLT blocksToLeave; // Datablock will after leaving this many blocks for this partition
LLT dbStart;
LLT maxInodesCount;

#endif
