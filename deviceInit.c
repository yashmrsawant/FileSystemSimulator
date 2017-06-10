/*
 * deviceInit.c
 *
 *  Created on: 19-May-2017
 *      Author: yashmrsawant
 */

#include <stdio.h>

#include "devinit.h"

int createFileSystem(FILE* metafile, int counter) {

	int sizeInMegabytes;
	struct devicelist dv;

	printf("Creating new Filesystem...\n");
	printf("Enter the size of Filesystem in Megabytes\n");
	scanf("%d", sizeInMegabytes);
	if(sizeInMegabytes >= 512 && sizeInMegabytes <= MAXALLOWEDSIZE) {

		if(metafile != NULL) {

			if(counter == 0) {
				// File contains no data
				fwrite(&counter, sizeof(int), 1, metafile);
			} else {
				fread(&counter, sizeof(int), 1, metafile);
			}

			//Seek to proper position
			fseek(metafile, counter * sizeof(struct devicelist), SEEK_CUR);

			counter = counter + 1;
			dv.devid = counter;
			dv.filename[0] = 's';
			dv.filename[1] = 'd';
			dv.filename[2] = 97 + counter - 1;
			dv.filename[3] = '\0';
			dv.partitions = 0;

			fwrite(&dv, sizeof(struct devicelist), 1, metafile);

		}
	}
}
