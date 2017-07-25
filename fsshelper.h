/*
 * fsshelper.h
 *
 *  Created on: 23-Jul-2017
 *      Author: yashmrsawant
 */

#ifndef FSSHELPER_H_
#define FSSHELPER_H_

#include "fssinit.h"

int writeInode(struct inode* inde_p, LLT inode);
void findInode(LLT* inodeN_p, char* word);
int allocateinode(struct superblock* sb_p, LLT* inodeN_p);
void pathToInode(char* path, LLT* inodeN_p);
int allocateblock(LLT* block_p, struct superblock* sb_p);
int inodeNumberToInode(LLT inode, struct inode* inode_p);
int assignBlockToInode(LLT inode, LLT block, struct superblock* sb_p);
BLOCK* readNextBlock(struct inode* inode_p, int index0, int index1, int index2,
                     int index3);

int writeDirectoryInfo(LLT parentInodeN, char* filename, LLT dirInodeN, struct superblock* sb_p);
void getDirectoryInfo(LLT inode, char* filename);
int writeDataFile(LLT inode, char* srcfilename, struct superblock* sb_p);
void changeToPartition(int id, struct superblock* sb_p);

#endif /* FSSHELPER_H_ */

