/*
 * devinit.h
 *
 *  Created on: 19-May-2017
 *      Author: yashmrsawant
 */

#ifndef DEVINIT_H_
#define DEVINIT_H_


#define MAXPARTITION 10
#define MAXFILENAMESIZE 4
#define MAXALLOWEDSIZE 4096 //In Megabytes

struct devicelist {

	int devid;
	int partitions;
	struct partitionList list[MAXPARTITION];
	char filename[MAXFILENAMESIZE];
};
struct partitionList {
	int partitionIndex;
	unsigned long long int bytePointer;
};


#endif /* DEVINIT_H_ */
