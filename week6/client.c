#include <stdio.h>
#include <stdlib.h>
#include <linux/msg.h>
#include <linux/ipc.h>

#define SERVER_HOME "/home/aleksander/Desktop/Spring14/concurrentProgramming/week6/"

typedef struct message {
	long id;
	char data[256];
} MESSAGE;

int main(int argc, char const *argv[])
{
	key_t inputKey = ftok(SERVER_HOME, 'i');
	key_t outputKey = ftok(SERVER_HOME, 'o');

	int inputQueue;
	int outputQueue;

	inputQueue=msgget(inputKey,0777);
	outputQueue=msgget(outputKey,0777);

	char * wordToAsk;
	MESSAGE msg;
	int id = getpid();
	printf("Podaj polskie słowo: \n");
	scanf("%s", msg.data);
	msg.id = id;
	msgsnd(inputQueue, &msg, sizeof(MESSAGE), 0);
	msgrcv(outputQueue, &msg, sizeof(MESSAGE), id, 0);
	printf("%s\n", msg.data);
	return 0;
}