/*
 * fssinit.h
 *
 *  Created on: 17-Jul-2017
 *      Author: yashmrsawant
 */
/*
 * fssinit.h
 *
 *  Created on: 12-Jul-2017
 *      Author: yashmrsawant
 */

#ifndef FSSINIT_H_
#define FSSINIT_H_

#include "commons.h"

#define BLOCKSIZE 4096 // In Bytes
#define FRILST 100 // Free Inode List
#define FILENAMESIZE 32
#define EXTRASTUFFSIZE 32 // 256 - (FILENAMESIZE + 13 * 8 + 1)
#define PARTITIONSIZE 4096 //In Megabytes
#define AVERAGEFILESIZE 2048 //In Kilobytes

#define BLOCKALLOCATIONSIZE 4

typedef unsigned long long int LLT;


//pthread_mutex_t directorylock;

struct superblock {

	// List of Free Inodes

	LLT free_inode_list[FRILST]; // Inode Number
	unsigned int next_ptr_free_inode;
	unsigned int next_free_inode_scanned;
	unsigned long int total_inodes;
	unsigned long int free_inodes;
	//pthread_mutex_t lock;
	LLT free_block_list[BLOCKSIZE/8];
	unsigned int free_block_list_ptr;
	LLT free_block_list_second_end_block; //
	LLT freeBlocks;
};

struct inode {
	char name[FILENAMESIZE];
	LLT data_blocks[13];
	char file_type;
	int lastIndex[4];
	LLT lastBlockpointers[4]; // Zeroth Indirect, First Indirect, Second Indirect, Third Indirect
	char extrastuff[EXTRASTUFFSIZE];
};

#define DIRWIDTH 32

struct dataFreeInfo {
	LLT free_blocks[BLOCKSIZE / 8];
};

struct directoryInfoBlock {
	LLT inode_Number[BLOCKSIZE/DIRWIDTH];
	char filename[BLOCKSIZE/DIRWIDTH][DIRWIDTH - 8];
};
typedef struct block {
	char characters[BLOCKSIZE];
} BLOCK;

int superblockinit(int argc, char** argv);

#endif /* FSSINIT_H_ */
