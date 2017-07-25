#include "fssserver.h"
#include "fsshelper.h"
#include "commons.h"

int writeInode(struct inode* inde_p, LLT inode) {


  fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (inode - 1)), SEEK_SET);
  fwrite(inde_p, sizeof(struct inode), 1, fss_area);
  return 0;
}

void findInode(LLT* inodeN_p, char* word) {
  //Read directory till 9 direct pointers
  struct inode inde;
  struct directoryInfoBlock* dirInfo_p;
  fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (*inodeN_p - 1)), SEEK_SET);
  fread(&inde, sizeof(struct inode), 1, fss_area);
  int i, j;
  int found = 0;
  for(i = 0 ; i <= 9 ; i ++) {
    dirInfo_p = (struct directoryInfoBlock*)(readNextBlock(&inde, i, 0, 0, 0));
    for(j = 0 ; j < BLOCKSIZE / DIRWIDTH ; j ++) {
      if(strcmp(word, dirInfo_p -> filename[j]) == 0) {
        *inodeN_p = dirInfo_p -> inode_Number[j];
        found = 1;
        break;
      }
    }
    if(found)
      break;
  }
  if(found == 0)
    *inodeN_p = 0;
}


int allocateinode(struct superblock* sb_p, LLT* inodeN_p) {

  struct inode inde;
  int i, j;
  if(sb_p -> next_ptr_free_inode < FRILST) {
    *inodeN_p = sb_p -> free_inode_list[sb_p -> next_ptr_free_inode];
    sb_p -> next_ptr_free_inode = sb_p -> next_ptr_free_inode + 1;
    fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (*inodeN_p - 1)), SEEK_SET);
    fread(&inde, sizeof(struct inode), 1, fss_area);
    inde.file_type = 'a';
    fseek(fss_area, sizeof(struct inode) * -1, SEEK_CUR);
    fwrite(&inde, sizeof(struct inode), 1, fss_area);

    //Test Code
    fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (*inodeN_p - 1)), SEEK_SET);
    fread(&inde, sizeof(struct inode), 1, fss_area);

  } else {
    j = sb_p -> next_free_inode_scanned;
    i = FRILST - 1;
    fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (j - 1)), SEEK_SET);
    while(j < sb_p -> free_inodes && sb_p -> next_ptr_free_inode > 0 ) {
      fread(&inde, sizeof(struct inode), 1, fss_area);
      if(inde.file_type == 0) {
        sb_p -> free_inode_list[i --] = j;
        sb_p -> next_ptr_free_inode = sb_p -> next_ptr_free_inode - 1;
      }
      j ++;
    }
    if(sb_p -> next_ptr_free_inode < FRILST) {
      *inodeN_p = sb_p -> free_inode_list[sb_p -> next_ptr_free_inode];
      sb_p -> next_ptr_free_inode = sb_p -> next_ptr_free_inode + 1;
      fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (*inodeN_p - 1)), SEEK_SET);
      fread(&inde, sizeof(struct inode), 1, fss_area);
      inde.file_type = 'a';
      fseek(fss_area,  sizeof(struct inode) * -1, SEEK_CUR);
      fwrite(&inde, sizeof(struct inode), 1, fss_area);
    } else
      return -1;
  }
  fseek(fss_area, BLOCKSIZE * (blocksToLeave), SEEK_SET);
  fwrite(sb_p, sizeof(struct superblock), 1, fss_area);
  return 1;
}
void pathToInode(char* path, LLT* inodeN_p) {
  *inodeN_p = 2;
  char word[FILENAMESIZE];
  int i = 0, j = 0;
  while(path[i] != '\0') {
    if(path[i] == '/') {
      if(i != 0 && j != 0) {
        word[j] = '\0';
        j = 0;
       }
    } else if((path[i] >= 'A' && path[i] <= 'Z') ||
              (path[i] >= 'a' && path[i] <= 'z') ||
              (path[i] >= '0' && path[i] <= '9')) {

                 word[j ++] = path[i];
      }
    if((j == 0 || path[i + 1] == '\0') && i != 0) {
        if(j != 0)
          word[j] = '\0';
        findInode(inodeN_p, word);
        if(*inodeN_p == 0)
          break;
    }
    i ++;
  }
}
int allocateblock(LLT* block_p, struct superblock* sb_p) {

	int i;
	struct dataFreeInfo dsInfo;
	if(sb_p -> free_block_list_ptr > 0) {
		*block_p = sb_p -> free_block_list[sb_p -> free_block_list_ptr];
		sb_p -> free_block_list_ptr = sb_p -> free_block_list_ptr - 1;
		sb_p -> freeBlocks = sb_p -> freeBlocks - 1;
		//printf("BlocksToLeave %d", blocksToLeave);
		fseek(fss_area, BLOCKSIZE * (blocksToLeave), SEEK_SET);
		fwrite(sb_p, sizeof(struct superblock), 1, fss_area);
		return 1;
	} else if(sb_p -> free_block_list_ptr == 0 && sb_p -> freeBlocks != 0) {
		fseek(fss_area, BLOCKSIZE * (blocksToLeave + (sb_p -> free_block_list[0]) - 1), SEEK_SET);
		fread(&dsInfo, sizeof(struct dataFreeInfo), 1, fss_area);
		*block_p = sb_p -> free_block_list[0];
		sb_p -> free_block_list[0] = dsInfo.free_blocks[0];
		for(i = 1 ; i < BLOCKSIZE / 8 ; i ++) {
			sb_p -> free_block_list[i] = dsInfo.free_blocks[i];
			if(dsInfo.free_blocks[i] != 0)
				sb_p -> free_block_list_ptr = i;
			else
				break;
		}

		fseek(fss_area, BLOCKSIZE * (blocksToLeave), SEEK_SET);
		fwrite(sb_p, sizeof(struct superblock), 1, fss_area);
		return 1;
	}
}
int inodeNumberToInode(LLT inode, struct inode* inode_p) {

	fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (inode - 1)), SEEK_SET);
	return fread(inode_p, sizeof(struct inode), 1, fss_area);
}

int assignBlockToInode(LLT inode, LLT block, struct superblock* sb_p) {

	struct inode inde;
	struct dataFreeInfo dsInfoI, dsInfoII;
	LLT block_t;

	inodeNumberToInode(inode, &inde);

	if(inde.lastBlockpointers[0] == 0) {
		inde.data_blocks[0] = block;
		inde.lastBlockpointers[0] = block;
		inde.lastIndex[0] = 0;
	} else if(inde.lastBlockpointers[1] == 0) {
		if(inde.lastIndex[0] < 9) {
			inde.data_blocks[++ inde.lastIndex[0]] = block;
			inde.lastBlockpointers[0] = block;
		} else {

			if(allocateblock(&block_t, sb_p) > 0) {
				memset(&dsInfoI, 0, sizeof(struct dataFreeInfo));
				dsInfoI.free_blocks[0] = block;
				fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
				fwrite(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);
				inde.data_blocks[10] = block_t;
				inde.lastBlockpointers[0] = block_t;
				inde.lastBlockpointers[1] = block;
				inde.lastIndex[0] = 10;
				inde.lastIndex[1] = 0;
			}
		}
	} else if(inde.lastBlockpointers[2] == 0) {
		block_t = inde.data_blocks[10];
		fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
		fread(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);
		if(inde.lastIndex[1] < (BLOCKSIZE / 8 - 1)) {
			dsInfoI.free_blocks[++ inde.lastIndex[1]] = block;
			fseek(fss_area, sizeof(struct dataFreeInfo) * -1, SEEK_CUR);
			fwrite(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);
			inde.lastBlockpointers[1] = block;
		} else {
			if(allocateblock(&block_t, sb_p) < 0)
				return -1;
			inde.data_blocks[11] = block_t;
			if(allocateblock(&block_t, sb_p) < 0)
				return -1;
			memset(&dsInfoI, 0, sizeof(struct dataFreeInfo));
			dsInfoI.free_blocks[0] = block_t;
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + inde.data_blocks[11] - 1), SEEK_SET);
			fwrite(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);

			memset(&dsInfoII, 0, sizeof(struct dataFreeInfo));
			dsInfoII.free_blocks[0]  = block;
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
			fwrite(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);

			inde.lastBlockpointers[0] = inde.data_blocks[11];
			inde.lastBlockpointers[1] = block_t;
			inde.lastBlockpointers[2] = block;
			inde.lastIndex[0] = 11;
			inde.lastIndex[1] = 0;
			inde.lastIndex[2] = 0;
		}
	} else if(inde.lastBlockpointers[3] == 0) {
		block_t = inde.data_blocks[11];
		fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
		fread(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);

		if(inde.lastIndex[1] < (BLOCKSIZE / 8 - 1)) {
			block_t = dsInfoI.free_blocks[inde.lastIndex[1]];
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
			fread(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);
			if(inde.lastIndex[2] < (BLOCKSIZE / 8 - 1)) {
				dsInfoII.free_blocks[++ inde.lastIndex[2]] = block;
				fseek(fss_area, sizeof(struct dataFreeInfo) * -1, SEEK_CUR);
				fwrite(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);

				inde.lastBlockpointers[2] = block;
			} else {
					if(allocateblock(&block_t, sb_p) < 0)
						return -1;
					dsInfoI.free_blocks[++ inde.lastIndex[1]] = block_t;
					fseek(fss_area, BLOCKSIZE * (blocksToLeave +  inde.lastBlockpointers[0] - 1), SEEK_SET);
					fwrite(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);

					memset(&dsInfoII, 0, sizeof(struct dataFreeInfo));
					dsInfoII.free_blocks[0] = block;
					fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
					fwrite(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);

					inde.lastBlockpointers[1] = block_t;
					inde.lastBlockpointers[2] = block;
					inde.lastIndex[2] = 0;
				}
		} else {
			// Case 11 the Direct has dsInfoI last index
			if(inde.lastIndex[2] < (BLOCKSIZE / 8 - 1)) {
				block_t = dsInfoI.free_blocks[inde.lastIndex[1]];
				fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
				fread(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);
				dsInfoII.free_blocks[++ inde.lastIndex[2]] = block;
				fseek(fss_area, sizeof(struct dataFreeInfo) * -1, SEEK_CUR);
				fwrite(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);

				inde.lastBlockpointers[2] = block;
			} else {
				return -1;
			}
		}
	} else {
		return -1;
		}
	writeInode(&inde, inode);
	return 1;
}

BLOCK* readNextBlock(struct inode* inode_p, int index0, int index1, int index2, int index3) {
	struct block* blk_p;
	struct dataFreeInfo dsInfoI;
	struct dataFreeInfo dsInfoII;
	struct dataFreeInfo dsInfoIII;
	blk_p = (struct block*)(malloc(sizeof(struct block)));
	memset(blk_p, 0, sizeof(struct block));
	if(index1 == 0) {
		if(index0 <= inode_p -> lastIndex[0]) {
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + inode_p -> data_blocks[index0] - 1), SEEK_SET);
			fread(blk_p, sizeof(struct block), 1, fss_area);

		}
	} else if(index2 == 0) {
		if(index0 == 10 && index1 <= inode_p -> lastIndex[1]) {
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + inode_p -> data_blocks[11] - 1), SEEK_SET);
			fread(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + dsInfoI.free_blocks[index1] - 1), SEEK_SET);
			fread(blk_p, sizeof(struct block), 1, fss_area);
		}
	} else if(index3 == 0) {
		if(index0 == 11 && index1 <= inode_p -> lastIndex[1]
			 && index2 <= inode_p -> lastIndex[2]) {
				fseek(fss_area, BLOCKSIZE * (blocksToLeave + inode_p -> data_blocks[11] - 1), SEEK_SET);
				fread(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);
				fseek(fss_area, BLOCKSIZE * (blocksToLeave + dsInfoI.free_blocks[index1] - 1), SEEK_SET);
				fread(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);
				fseek(fss_area, BLOCKSIZE * (blocksToLeave + dsInfoII.free_blocks[index2] - 1), SEEK_SET);
				fread(blk_p, sizeof(struct block), 1, fss_area);
			 }
	} else {
		if(index0 == 12 && index2 <= inode_p -> lastIndex[1]
				&& index2 <= inode_p -> lastIndex[2] && index3 <= inode_p -> lastIndex[3]) {
					fseek(fss_area, BLOCKSIZE * (blocksToLeave + inode_p -> data_blocks[12] - 1), SEEK_SET);
					fread(&dsInfoI, sizeof(struct dataFreeInfo), 1, fss_area);
					fseek(fss_area, BLOCKSIZE * (blocksToLeave + dsInfoI.free_blocks[index1] - 1), SEEK_SET);
					fread(&dsInfoII, sizeof(struct dataFreeInfo), 1, fss_area);
					fseek(fss_area, BLOCKSIZE * (blocksToLeave + dsInfoII.free_blocks[index2] - 1), SEEK_SET);
					fread(&dsInfoIII, sizeof(struct dataFreeInfo), 1 ,fss_area);
					fseek(fss_area, BLOCKSIZE * (blocksToLeave + dsInfoIII.free_blocks[index3] - 1), SEEK_SET);
					fread(blk_p, sizeof(struct block), 1, fss_area);
				}
	}
  return blk_p;
}

int writeDirectoryInfo(LLT parentInodeN, char* filename, LLT dirInodeN, struct superblock* sb_p) {

	struct inode parentInode;
	struct directoryInfoBlock* dirInfo_p;
	struct block blk;
	int i, j;
	int found = -1;
	LLT block_t;
	fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (parentInodeN - 1)), SEEK_SET);
	fread(&parentInode, sizeof(struct inode), 1, fss_area);
	for(i = 0 ; i < 9 ; i ++) {
		if(parentInode.data_blocks[i] == 0) {
			if(allocateblock(&block_t, sb_p) < 0)
				return -1;
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + block_t - 1), SEEK_SET);
			memset(&blk, 0, sizeof(struct block));
			fwrite(&blk, sizeof(struct block), 1, fss_area);
			assignBlockToInode(parentInodeN, block_t, sb_p);
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (parentInodeN - 1)), SEEK_SET);
			fread(&parentInode, sizeof(struct inode), 1, fss_area);
		}
		dirInfo_p = (struct directoryInfoBlock*)(readNextBlock(&parentInode, i, 0, 0, 0));
		for(j = 0 ; j < BLOCKSIZE / DIRWIDTH ; j ++) {
			if(dirInfo_p -> inode_Number[j] == 0) {
				found = j;
				break;
			}
		}
		if(found != -1) {
			strcpy(dirInfo_p -> filename[found], filename);
			dirInfo_p -> inode_Number[found] = dirInodeN;
			fseek(fss_area, BLOCKSIZE * (blocksToLeave + parentInode.data_blocks[i] - 1), SEEK_SET);
			fwrite(dirInfo_p, sizeof(struct directoryInfoBlock), 1, fss_area);
			break;
		}
	}
	//writeInode(&parentInode, parentInodeN);
}

int writeDataFile(LLT inode, char* srcfilename, struct superblock* sb_p) {

	struct stat buf;
	struct block blk;
	LLT block_t;
	int n;
	FILE* srcfile;
	if(stat(srcfilename, &buf) < 0)
		return -1;

	n = buf.st_size / BLOCKSIZE;
	srcfile = fopen(srcfilename, "r+b");
	int i = 0;
	if(n == 0) {
		while((blk.characters[i ++] = fgetc(srcfile)) != EOF);
		if(allocateblock(&block_t, sb_p) < 0)
			return -1;
		assignBlockToInode(inode, block_t, sb_p);
	} else {
		fseek(srcfile, 0, SEEK_SET);
		for(i = 0 ; i < n - 1 ; i ++) {
			fread(&blk, sizeof(struct block), 1, srcfile);
			if(allocateblock(&block_t, sb_p) < 0)
				return -1;
			assignBlockToInode(inode, block_t, sb_p);
		}
		i = 0;
		while((blk.characters[i ++] = fgetc(srcfile)) != EOF);
		if(allocateblock(&block_t, sb_p) < 0)
			return -1;
		assignBlockToInode(inode, block_t, sb_p);
	}
}
void changeToPartition(int id, struct superblock* sb_p) {
	//Dummy Code
	blocksToLeave = 10 + id;	
	fseek(fss_area, BLOCKSIZE * (blocksToLeave), SEEK_SET);
	fread(sb_p, sizeof(struct superblock), 1, fss_area);
}

