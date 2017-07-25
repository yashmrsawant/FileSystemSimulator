#include "fssserver.h"
#include "commons.h"
#include "stack.h"
#include "fsshelper.h"

#define MAX_THREADS 5
#define PORT 8888

char rbuf[5][1024];
char rbufA[5][1024];
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

void ls(char* path) {
	LLT inodeN;
	pathToInode(path, &inodeN);

}
void mkfile(char* despath, char* srcpath) {

	printf("%s\n", despath);
}
void mkdir(char* despath) {
	printf("%s\n", despath);
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
		ls(rbuf[info_p -> index]);
		pthread_mutex_unlock(&superLock);

	} else if(!strcmp(rbuf[info_p -> index], "mkfile")) {
		read(info_p -> sockt_cl, rbuf[info_p -> index], 1024);
		read(info_p -> sockt_cl, rbufA[info_p -> index], 1024);
		pthread_mutex_lock(&superLock);
		mkfile(rbuf[info_p -> index], rbufA[info_p -> index]);
		pthread_mutex_unlock(&superLock);
	} else if(!strcmp(rbuf[info_p -> index], "mkdir")) {
		read(info_p -> sockt_cl, rbuf[info_p -> index], 1024);

		pthread_mutex_lock(&superLock);
		mkdir(rbuf[info_p -> index]);
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
	listen(sockt_fd, MAX_THREADS);
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
