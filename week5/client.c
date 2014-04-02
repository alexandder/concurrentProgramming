#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>


typedef struct clientmessage
{
   int len;
   int id;
   char home[1000];
}CLIENTMESSAGE;

typedef struct servermessage
{
   int len;
   char answer[1000];
}SERVERMESSAGE;

int main(int argc, char const *argv[])
{


	int fds;
	fds = open("/home/aleksander/Desktop/Spring14/concurrentProgramming/week5/serwerfifo", O_WRONLY, 0);
	CLIENTMESSAGE cmsg;
	int id;
	printf("Podaj id: \n");
	scanf("%d", &id);
	getcwd(cmsg.home, sizeof(cmsg.home));
	cmsg.id = id;
	cmsg.len = sizeof(int) + (int)(strlen(cmsg.home) + sizeof(int));
	write(fds, &cmsg, sizeof(cmsg));
	mkfifo("klientfifo", S_IRWXU);
	int fdc = open("klientfifo",O_RDONLY, 0);
	SERVERMESSAGE smsg;
	read(fdc, &smsg, sizeof(smsg));
	printf("%s\n", smsg.answer);
	unlink("klientfifo");
	close(fdc);
	close(fds);
	return 0;
}