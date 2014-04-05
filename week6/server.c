#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/msg.h>
#include <linux/ipc.h>
#include <unistd.h>
#include <signal.h>

#define SIZE 8
#define SERVER_HOME "/home/aleksander/Desktop/Spring14/concurrentProgramming/week6/"

typedef struct message {
	long id;
	char data[256];
} MESSAGE;

typedef struct word {
	char * pl;
	char * eng;
} WORD;

int waitForMessage = 1;

char* findWord(char * plWord, WORD words[]) {
	int i;
	for (i = 0; i < SIZE; i++) {
		if (strcmp(plWord, words[i].pl) == 0) {
			return words[i].eng;
		}
	}
	return "Nie ma takiego słowa.";
}


void sigintHandler(int sig) {
	waitForMessage = 0;
	putchar('\n');
}

int main(int argc, char const *argv[])
{

	char * plWords[SIZE] = {"kwiecień", "dom", "szkoła", "czerwony", "niebieski", "drzewo",
							"suszarka", "mózg"};
	char * engWords[SIZE] = {"April", "house", "school", "red", "blue", "tree",
							"hair dryer", "brain"};
	
	WORD words[SIZE];
	int i;
	for (i = 0; i < SIZE; i++) {
		words[i].pl = plWords[i];
		words[i].eng = engWords[i];
	}

	key_t inputKey = ftok(SERVER_HOME, 'i');
	key_t outputKey = ftok(SERVER_HOME, 'o');
	int inputQueue;
	int outputQueue;

	inputQueue=msgget(inputKey,0777|IPC_CREAT);
	outputQueue=msgget(outputKey,0777|IPC_CREAT);

	signal(SIGINT, sigintHandler);

	while(waitForMessage) {
		MESSAGE msg;
		msgrcv(inputQueue, &msg, sizeof(MESSAGE), 0, 0);
		char * answer = findWord(msg.data, words);
		strcpy(msg.data, answer);
		msgsnd(outputQueue, &msg, sizeof(MESSAGE), 0);
	}

	msgctl(inputQueue,IPC_RMID,0);
	msgctl(outputQueue,IPC_RMID,0);
	return 0;
}