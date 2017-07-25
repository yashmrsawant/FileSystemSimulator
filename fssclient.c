#include "commons.h"

#define PORT 8888

int main(int argc, char** argv) {

	int sockt_fd;
	int connection;
	struct sockaddr_in servSocket;
	char message[1024];
	int choice;
	
	servSocket.sin_addr.s_addr = inet_addr("127.0.0.1");
	servSocket.sin_family = AF_INET;
	servSocket.sin_port = htons(PORT);
	
	sockt_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockt_fd < 0) {
		perror("Erro Creating Socket");
		exit(-1);
	}
	connection = connect(sockt_fd, (struct sockaddr *)(&servSocket), sizeof(struct sockaddr_in));
	if(connection < 0) {
		perror("Error Connecting Server");
		exit(-1);
	}
	printf("Enter the Choice\n");
	printf("1) ls 2) mkfile 3) mkdir\n");
	scanf("%d", &choice);

	if(choice == 1) {
		strcpy(message, "ls");
		send(sockt_fd, message, 3, 0);
		fscanf(stdin, "%s", message);
		message[1023] = '\0';
		send(sockt_fd, message, strlen(message) + 1, 0);
	} else if(choice == 2) {
		strcpy(message, "mkfile");
		send(sockt_fd, message, 7, 0);
		fscanf(stdin, "%s", message);
		message[1023] = '\0';
		send(sockt_fd, message, strlen(message) + 1, 0);
		fscanf(stdin, "%s", message);
		message[1023] = '\0';
		send(sockt_fd, message, strlen(message) + 1, 0);
	} else if(choice == 3) {
		strcpy(message, "mkdir");
		send(sockt_fd, message, 6, 0);
		fscanf(stdin, "%s", message);
		message[1023] = '\0';
		send(sockt_fd, message, strlen(message) + 1, 0);
	}
	read(sockt_fd, message, 1024);
	printf("%s", message);
	close(sockt_fd);
	return 0;
}
