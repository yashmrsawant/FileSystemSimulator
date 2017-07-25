#include "commons.h"
#include "fssinit.h"
#include "fsshelper.h"
#include "fssserver.h"

int print(struct superblock* sb_p) {
	int i = 0, j = 0;
	int dbNeed, dbTotal;
	struct dataFreeInfo dsInfo;
	
	dbTotal = PARTITIONSIZE * 1024 / (BLOCKSIZE / 1024) - dbStart + 1;
	printf("No. of blocks %d\n", sb_p -> freeBlocks);
	printf("No. of Inodes %d\n", sb_p -> total_inodes);
	printf("Free Block List Ptr %d\n", sb_p -> free_block_list_ptr);
	for(i = 0 ; i < BLOCKSIZE / 8 ; i ++) {
		printf("%d ", sb_p -> free_block_list[i]);
	}
	printf("\n");
	
	fseek(fss_area, BLOCKSIZE * (blocksToLeave + sb_p -> free_block_list[0] - 1), SEEK_SET);
	printf("blocksToLeave = %d\n", blocksToLeave);
	printf("dbStart = %d\n", dbStart);
	printf("Inode Size = %d\n", sizeof(struct inode));
	dbNeed = ceil(dbTotal * 8.0 / BLOCKSIZE) - 1;
	for(i = 0 ; i < 1 ; i ++) {	
		fread(&dsInfo, sizeof(struct dataFreeInfo), 1, fss_area);
		for(j = 0 ; j < BLOCKSIZE / 8 ; j ++){
			printf("%d ", dsInfo.free_blocks[j]);
		}
		printf("\n");
	}
	return 0;
}

int main(int argc, char** argv) {

	struct superblock sb;
	fss_area = fopen(argv[1], "r+b");
	changeToPartition(0, &sb); 
	print(&sb);	


	fclose(fss_area);

	

	return 0;
}

