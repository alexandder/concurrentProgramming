#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

int main(int argc, char const *argv[])
{
	int socketd;
	int connectStatus;
	struct sockaddr_in server;

	char buf[256];

	socketd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketd == -1) {
		perror("Socket creation");
		exit(1);
	}

	server.sin_family = AF_INET;
	inet_aton("153.19.1.202", &server.sin_addr);
	server.sin_port = htons(5000);

	connectStatus = connect(socketd, (struct sockaddr*)&server, sizeof(server));
	if (connectStatus == -1) {
		printf("AAA\n");
		perror("Connection error");
		exit(1);
	}
	int number;
	printf("Podaj liczbę do wysłania: ");
	scanf("%d", &number);
	write(socketd, &number, 4);
	printf("\n");
	read(socketd, &buf, 4);
	printf("%s\n", buf);


	return 0;
}