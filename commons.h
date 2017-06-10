/*
 * commons.h
 *
 *  Created on: 10-Jun-2017
 *      Author: yashmrsawant
 */

#ifndef COMMONS_H_
#define COMMONS_H_


#include <stdio.h>
#include <semaphore.h>
#include <math.h>

#define BLOCKSIZE 4096 // In Bytes
#define FRILST 10 // Free Inode List
#define FILENAMESIZE 32
#define EXTRASTUFFSIZE 80 // 256 - (FILENAMESIZE + 13 * 8 + 1)
#define PARTITIONSIZE 512 //In Megabytes
#define AVERAGEFILESIZE 2048 //In Kilobytes

#define BLOCKALLOCATIONSIZE 4

typedef unsigned long long int LLT;

struct superblock {

	// List of Free Inodes

	unsigned int free_inode_list[FRILST]; // Inode Number
	unsigned int next_ptr_free_inode;
	unsigned int next_free_inode_scanned;
	sem_t mutex;
	unsigned long int total_inodes;
	unsigned long int free_inodes;
	LLT free_block_list[BLOCKSIZE/8];
	unsigned int free_block_list_ptr;
	LLT free_block_list_second_end_block; //
	LLT freeBlocks;
};


struct inode {
	char name[FILENAMESIZE];
	LLT data_blocks[13];
	char file_type;
	int  lastIndex;
	LLT lastBlockpointers[4];
	char extrastuff[EXTRASTUFFSIZE];
};

LLT blockNumber;
LLT maxInodesCount = 2 * PARTITIONSIZE * 1024 / AVERAGEFILESIZE;

#endif /* COMMONS_H_ */
