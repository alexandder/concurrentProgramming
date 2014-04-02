#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct person
{
	int ID;
	char *nazwisko;
}PERSON;

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

char * findPerson(int id, PERSON persons[]) {
	int i;
	for (i = 0; i < 20; i++) {
		if (id == i + 1) {
			return persons[i].nazwisko;
		}
	}
	return "Nie ma";
}

int main(int argc, char const *argv[])
{
	
	char* lastNames[20] = {"Smith", "Johnson", "Williams", "Jones", "Brown",
						"Davis", "Miller", "Wilson", "Moore", "Taylor",
						"Anderson", "Thomas", "Jackson", "White", "Harris",
						"Martin", "Thompson", "Garcia", "Martinez", "Robinson"};
	PERSON persons[20];
	int i;
	for (i = 0; i < 20; i++) {
		persons[i].ID = i+1;
		persons[i].nazwisko = lastNames[i];
	}

	mkfifo("serwerfifo", S_IRWXU);	
	int fds;
	CLIENTMESSAGE cmsg;
	int res;
	fds = open("serwerfifo",O_RDONLY, 0);
	int fdc;
	while(1) {
		res = read(fds, &cmsg, sizeof(cmsg));
		if (res > 0) {
			char * answer = findPerson(cmsg.id, persons);
			SERVERMESSAGE smsg;
			strcpy(smsg.answer, answer);
			smsg.len = 4 + sizeof(answer);
			char *clientFifo = (char *) malloc(1 + strlen(cmsg.home) + strlen("/klientfifo"));
			strcpy(clientFifo, cmsg.home);
			strcat(clientFifo, "/klientfifo");
			fdc = open(clientFifo, O_WRONLY, 0);
			write(fdc, &smsg, sizeof(smsg));
			close(fdc);
		}
	}

	return 0;
}
