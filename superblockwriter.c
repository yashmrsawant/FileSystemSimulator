#include "commons.h"
#include "fssinit.h"
#include "fssserver.h"
#include "fsshelper.h"

void writesuperblock() {
	int i, j;
	/**
		 *  @dbNeed : total data blocks needed to
		 *  		store all data block info
		 *  @dBLast : Last data block number
		 */
	LLT dBNeed;
	LLT dBTotal; //Total Data Blocks
	LLT dBLast;
	LLT maxInodesCount;	
	struct superblock sb;
	struct dataFreeInfo dsInfo;
	memset(&sb, 0, sizeof(struct superblock));
	for(i = 0; i < FRILST ; i ++) {
		sb.free_inode_list[i] = i + 1;
	}
	sb.next_ptr_free_inode = 2;
	maxInodesCount = 2 * PARTITIONSIZE * 1024 / AVERAGEFILESIZE;
	if(FRILST < maxInodesCount)
		sb.next_free_inode_scanned = FRILST + 1;
	else
		sb.next_free_inode_scanned = maxInodesCount + 1;
	sb.total_inodes = maxInodesCount;
	sb.free_inodes = sb.total_inodes;
	
	

	dbStart = ceil((sizeof(struct superblock) + maxInodesCount * sizeof(struct inode)) / (BLOCKSIZE * 1.0)) + 1;
	dBTotal = PARTITIONSIZE * 1024 / (BLOCKSIZE / 1024) - dbStart + 1;
	dBNeed = ceil(dBTotal * 8.0 / BLOCKSIZE) - 1;

	sb.free_block_list[0] = dbStart;
	sb.freeBlocks = dBTotal;
	dBLast = dBNeed + dbStart;

	if(dBNeed > 0) {
		for(i = 1 ; i < BLOCKSIZE / 8 ; i ++) {
			sb.free_block_list[i] = dBLast ++;
			sb.free_block_list_ptr = i;
		}
	} else {
		for(i = 1 ; i < BLOCKSIZE / 8 ; i ++) {
			if(dBTotal >= dBLast) {
				sb.free_block_list[i] = dBLast ++;
				sb.free_block_list_ptr = i;
			} else
				break;
		}
	}
	fseek(fss_area, BLOCKSIZE * (blocksToLeave), SEEK_SET);
	fwrite(&sb, sizeof(struct superblock), 1, fss_area);

	fseek(fss_area, BLOCKSIZE * (blocksToLeave + dbStart - 1), SEEK_SET);
	for(i = dbStart ; i < (dBNeed + dbStart - 1) ; i ++) {
		dsInfo.free_blocks[0] = i + 1;
		for(j = 1 ; j < BLOCKSIZE / 8 ; j ++) {
			dsInfo.free_blocks[j] = dBLast ++;
		}
		fwrite(&dsInfo, sizeof(struct dataFreeInfo), 1, fss_area);
	}
	memset(&dsInfo, 0, sizeof(struct dataFreeInfo));
	for(j = 1 ; j < BLOCKSIZE / 8 ; j ++) {
		if(dBTotal >= dBLast) {
			dsInfo.free_blocks[j] = dBLast ++;
		} else
			break;
	}
	fwrite(&dsInfo, sizeof(struct dataFreeInfo), 1, fss_area);
	fseek(fss_area, BLOCKSIZE * (blocksToLeave + dBLast - 2), SEEK_SET);

	memset(&dsInfo, 0, sizeof(struct dataFreeInfo));
	fwrite(&dsInfo, sizeof(struct dataFreeInfo), 1, fss_area);
}

void writeinodes() {
	int i;
	struct inode inde;
	fseek(fss_area, BLOCKSIZE * (blocksToLeave) + sizeof(struct superblock), SEEK_SET);
	memset(&inde, 0, sizeof(struct inode));
	for(i = 0 ; i < maxInodesCount ; i ++)
		fwrite(&inde, sizeof(struct inode), 1, fss_area);
}
/**
 *  @argv
 *  	filename<with path>
 */
int main(int argc, char** argv) {

	struct superblock sb;
	fss_area = fopen(argv[1], "w+b");
	fclose(fss_area);
	changeToPartition(0, &sb); 
	
	fss_area = fopen(argv[1], "r+b");
	writesuperblock(fss_area);
	writeinodes(fss_area);
	fclose(fss_area);
	return 0;
}

