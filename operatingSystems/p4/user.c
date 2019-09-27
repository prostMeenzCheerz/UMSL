#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "header.h"

int main(int argc, char *argv[]) {

	srand(getpid());

	//shared mem pointers for the clock
	unsigned int *seconds = 0, *milliseconds = 0, actionSeconds = 0, actionMilli = 0;

	//shared mem pointer to array of user structs
	userPCB *userPcbArrPtr = NULL;

	//instance of message queue
	msg message = {.msg_type = 1, .msg_txt = {0}};
	term termMessage = {.term_type = 2, .term_txt = {0}};

	//values passed to user through exec
	int clockMemLocation = atoi(argv[1]), msgID = atoi(argv[2]), termID = atoi(argv[3]), shPcbMem = atoi(argv[4]), pos = atoi(argv[5]), move = 0, randomResource = 0;
	char bound[100] = {0}, choice[10] = {0};
	strcpy(bound, argv[5]);

	pointToSharedMem(&seconds, &milliseconds, &userPcbArrPtr, clockMemLocation, shPcbMem);
	init_user(userPcbArrPtr, pos);

	while(1) {
		while(userPcbArrPtr[pos].isBlocked == 1);
		pickATime(seconds, milliseconds, &actionSeconds, &actionMilli);
		while((*seconds > actionSeconds) || (*seconds == actionSeconds && *milliseconds >= actionMilli));
		move = action(userPcbArrPtr, pos);
		if(move == 0) {
			snprintf(termMessage.term_txt, 100, "%d ", getpid());
			char position[10] = {0};
			snprintf(position, 11, "%d", pos);
			strcat(termMessage.term_txt, position);
			msgsnd(termID, &termMessage, sizeof(termMessage), 0);
			break;
		}
		else if(move == 1) {
			getRequestedResource(userPcbArrPtr, pos, &randomResource);
			snprintf(message.msg_txt, 100, "%d ", pos);
			strcat(message.msg_txt, "1 ");
			snprintf(choice, 11, "%d", randomResource);
			strcat(message.msg_txt, choice);
			msgsnd(msgID, &message, sizeof(message), 0);
			memset(choice, 0, sizeof(choice));
		}
		else if(move == 2) {
			getReleasedResource(userPcbArrPtr, pos, &randomResource);
			snprintf(message.msg_txt, 100, "%d ", pos);
			strcat(message.msg_txt, "2 ");
			snprintf(choice, 11, "%d", randomResource);
			strcat(message.msg_txt, choice);
			memset(choice, 0, sizeof(choice));
			msgsnd(msgID, &message, sizeof(message), 0);
		}
	}
	unattachFromMem(seconds, userPcbArrPtr);
	return 0;
};
