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

	int buf;

	socketd = socket(AF_INET, SOCK_DGRAM, 0);
	if (socketd == -1) {
		perror("Socket creation");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr("153.19.1.240");
	server.sin_port = htons((ushort) 5000);

	connectStatus = connect(socketd, (struct sockaddr*)&server, sizeof(server));
	if (connectStatus == -1) {
		perror("Connection error");
		exit(1);
	}
	int slen = sizeof(server);
	printf("Podaj liczbę do wysłania: ");
	scanf("%d", &buf);
	printf("%d\n", buf);
	int wres = sendto(socketd, (char *)&buf, sizeof(int), 0, (struct sockaddr *)&server, slen);
       

	//printf("write result: %d\n", wres);
	
	int rres = recvfrom(socketd, (char *)&buf, sizeof(int), 0, (struct sockaddr *)&server, &slen);
	//printf("read result: %d\n", rres);
	
	printf("%d\n", buf);
	close(socketd);
	return 0;
}
