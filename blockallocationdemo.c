#include "commons.h"
#include "fsshelper.h"
#include "fssserver.h"

int main(int argc, char** argv) {

	struct superblock sb;
	LLT block;
	int i;
	FILE* fss;
	fss = fopen(argv[1], "r+b");
	if(fss != NULL) {
		changeToPartition(0, &sb, fss);
		for(i = 0 ; i < 500 ; i ++) {
		//lock		
			allocateblock(&block, fss, &sb);
		//unlock
		}
	}
	fclose(fss);
	return 0;
}
