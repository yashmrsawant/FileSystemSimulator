/*
 * main.c
 *
 *  Created on: 13-May-2017
 *      Author: yashmrsawant
 */


#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

FILE* metafile;
int counter;
struct stat buf;

int main(int argc, char ** argv) {


	metafile = fopen("metafile.bin", "wb");

	if(stat(metafile, &buf) < 0)
		return -1;

	counter = -1;
	if(buf.st_size == 0)
		counter = 0;
}

