#include "fssserver.h"
#include "commons.h"
#include "stack.h"
#include "fsshelper.h"

#define MAX_THREADS 5
#define PORT 8888

char rbuf[5][1024];
char rbufA[5][1024];
char rbufB[5][1024];
Stack stack;
struct thread_info {
	pthread_t id;
	int index;
	int sockt_cl;
};

pthread_mutex_t bufLock[5];
pthread_mutex_t stackLock;
pthread_mutex_t superLock;
pthread_attr_t attr;

struct superblock sb;


char fssname[5];

void ls(int sockt_cl, char* path) {
	int i, j;
	LLT inodeN;
	struct directoryInfoBlock* dirInfo_p;
	struct inode inde;
	pathToInode(path, &inodeN);
	fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (inodeN - 1)), SEEK_SET);
	fread(&inde, sizeof(struct inode), 1, fss_area);
	for(i = 0 ; i < 9 ; i ++) {
		if(inde.data_blocks[i] != 0) {
			dirInfo_p = (struct directoryInfoBlock *)(readNextBlock(&inde, i, 0, 0, 0));
			for(j = 0 ; j < BLOCKSIZE / DIRWIDTH ; j ++) {
				if(dirInfo_p -> inode_Number[j] != 0) {
					send(sockt_cl, dirInfo_p -> filename[j], 1024, 0);
				}
			}
		} else {
			break;
		}
	}
}
void makefile(int sockt_cl, char* despath, char* srcpath, char* filename) {
	LLT inodeN, parentInodeN;
	if(allocateinode(&sb, &inodeN) < 0)
		return;
	struct inode inde;
	fseek(fss_area, BLOCKSIZE * (blocksToLeave + sizeof(struct superblock) + sizeof(struct inode) * (inodeN - 1)), SEEK_SET);
	fread(&inde, sizeof(struct inode), 1, fss_area);
	inde.file_type = 'f';
	writeInode(&inde, inodeN);
	pathToInode(despath, &parentInodeN);
	writeDataFile(inodeN, parentInodeN, srcpath, &sb, filename);

	send(sockt_cl, "Successful", 32, 0);
}
void makedir(int sockt_cl, char* despath, char* dirname) {
	LLT inodeN, parentInodeN;
	if(allocateinode(&sb, &inodeN) < 0)
		return;
	pathToInode(despath, &parentInodeN);
	if(writeDirectoryInfo(parentInodeN, dirname, inodeN, &sb) < 0)
		return;
	send(sockt_cl, "Successful", 32, 0);
}

void* fsss(void* arg) {
	struct thread_info* info_p = (struct thread_info*)(arg);

	strcpy(rbuf[info_p -> index], "FS ");
	strncat(rbuf[info_p -> index], fssname, 5);
	strcat(rbuf[info_p -> index], " Partition Area Default");

	send(info_p -> sockt_cl, rbuf[info_p -> index], 1024, 0);

	read(info_p -> sockt_cl, rbuf[info_p -> index], 1024);
	if(!strcmp(rbuf[info_p -> index], "ls")) {
		read(info_p -> sockt_cl, rbuf[info_p -> index], 1024);
		pthread_mutex_lock(&superLock);
		ls(info_p -> sockt_cl, rbuf[info_p -> index]);
		pthread_mutex_unlock(&superLock);

	} else if(!strcmp(rbuf[info_p -> index], "mkfile")) {
		read(info_p -> sockt_cl, rbuf[info_p -> index], 1024);
		read(info_p -> sockt_cl, rbufA[info_p -> index], 1024);
		read(info_p -> sockt_cl, rbufB[info_p -> index], 1024);
		pthread_mutex_lock(&superLock);
		makefile(info_p -> sockt_cl, rbuf[info_p -> index], rbufA[info_p -> index], rbufB[info_p -> index]);
		pthread_mutex_unlock(&superLock);
	} else if(!strcmp(rbuf[info_p -> index], "mkdir")) {
		read(info_p -> sockt_cl, rbuf[info_p -> index], 1024);
		read(info_p -> sockt_cl, rbufA[info_p -> index], 1024);
		pthread_mutex_lock(&superLock);
		makedir(info_p -> sockt_cl, rbuf[info_p -> index], rbufA[info_p -> index]);
		pthread_mutex_unlock(&superLock);
	}
	pthread_mutex_lock(&stackLock);
	push(&stack, info_p -> index);
	pthread_mutex_unlock(&stackLock);

	close(info_p -> sockt_cl);
	return (void*)(info_p);
}

int alloc() {
	int z;
	if(stack.top != -1) {
		pthread_mutex_lock(&stackLock);
		z = pop(&stack);
		pthread_mutex_unlock(&stackLock);
		return z;
	}
	return -1;
}

void startFSSServer(int sockt_fd, struct sockaddr_in* localSocket_p) {
	int i = 0, s;
	struct thread_info* threads;
	struct sockaddr_in client;
	int socksize;
	int sockt_cl;

	threads = (struct thread_info*)(malloc(sizeof(struct thread_info) * MAX_THREADS));
	for(; i < MAX_THREADS ; i ++) {
		threads[i].index = i;
	}

	s = pthread_attr_init(&attr);
	if(s != 0)
		perror("Error initializing attributes");
	initializeStack(&stack, MAX_THREADS);
	for(i = 0 ; i < MAX_THREADS ; i ++) {
		push(&stack, i);
	}
	pthread_mutex_init(&stackLock, NULL);
	for(i = 0 ; i < MAX_THREADS ; i ++) {
		pthread_mutex_init(&bufLock[i], NULL);
	}
	pthread_mutex_init(&superLock, NULL);
	listen(sockt_fd, 1);
	while(1) {
		sockt_cl = accept(sockt_fd, (struct sockaddr*)(&client), (socklen_t*)(&socksize));
		while(stack.top == -1);
		s = alloc();
		threads[s].sockt_cl = sockt_cl;
		s = pthread_create(&(threads[s].id), &attr, &fsss, &threads[s]);
	}
}



int main(int argc, char** argv) {

	int sockt_fd, opt = 1;
	struct sockaddr_in localSocket;


	fss_area = fopen(argv[1], "r+b");
  	strcpy(fssname, argv[1]);
	printf("Changing to Given File Area and Partition 0\n");
	if(fss_area != NULL) {
		changeToPartition(0, &sb);
		memset(&localSocket, 0, sizeof(struct sockaddr_in));
		localSocket.sin_addr.s_addr = INADDR_ANY;
		localSocket.sin_family = AF_INET;
		localSocket.sin_port = htons(PORT);

		sockt_fd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockt_fd < 0) {
			perror("Error Creating Socket");
			exit(-1);
		}
		if(bind(sockt_fd, (struct sockaddr *)(&localSocket), sizeof(localSocket)) < 0) {
			perror("Error Binding Socket");
			exit(-1);
		}
		if(setsockopt(sockt_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
			perror("Error setting socket option");
			exit(-1);
		}
		startFSSServer(sockt_fd, &localSocket);
	}
}
