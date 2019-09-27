#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "header.h"

int main(int argc, char *argv[]) {

	srand(getpid());
	bool terminate = false;
	int msgID = atoi(argv[1]), memID = atoi(argv[2]), semID = atoi(argv[3]), readWrite = 0, requestMem = 0, needToRequest = 0, requestNum = 1, msgSent = 0;
	unsigned int *seconds = 0, *nanoseconds = 0, requestSeconds, requestNano;
	sem_t *sem = 0;
	char memRequest[10] = {0}, requestType[1] = {0};

	pointToShMem(&seconds, &nanoseconds, &sem, memID, semID);
	msgQ msg = {.msg_type = 0, .msg_txt = {0}};
	long int sendMsgType = getpid();
	long int rcvMsgType = sendMsgType * sendMsgType;
int sent = 0;
	while(1) {
		if((requestNum > 1) && (requestNum % 1000 == 0)) {
			terminate = termCheck();
			if(terminate == 1) {
				break;
			}
		}
		if(needToRequest == 0) {
			getRequestTime(seconds, nanoseconds, &requestSeconds, &requestNano);
			needToRequest = 1;
		
		while((*seconds < requestSeconds) || ((*seconds == requestSeconds) && (*nanoseconds <= requestNano)));
			makeRequest(&readWrite, &requestMem);
			buildSendRequest(&msg, sendMsgType, readWrite, requestMem, requestType, memRequest);
			msgsnd(msgID, &msg, sizeof(msg), 0);
			sent++;
			msgSent = 1;
			requestNum++;
			needToRequest = 0;

			if(msgSent == 1) {
			//waiting on OSS to handle the request
			msgrcv(msgID, &msg, sizeof(msg), rcvMsgType, 0);
			msgSent = 0;
			}
		}
	}
	msg.msg_type = sendMsgType;
	sendTermRequest(&msg, sendMsgType);
	msgsnd(msgID, &msg, sizeof(msg), IPC_NOWAIT);
	msgrcv(msgID, &msg, sizeof(msg), rcvMsgType, 0);
	return 0;
}
