#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "header.h"

void getRequestTime(unsigned int * seconds, unsigned int *nanoseconds, unsigned int *requestSeconds, unsigned int *requestNano) {
	//wait till a certain time passes before making another request
	*requestSeconds = seconds[0];
	*requestNano = nanoseconds[0];

	unsigned int randNum = ((rand() % 7500000000) + 1);
	*requestNano += randNum;
	
	if(*requestNano > 1000000000) {
		(*requestSeconds)++;
		*requestNano = *requestNano - 1000000000;
	}
	else if(*requestNano == 1000000000) {
		(*requestSeconds)++;
		*requestNano = 0;
	}
};

void makeRequest(int *readWrite, int *requestMem) {
	//is it read or write request?
	*readWrite = ((rand() % 100) + 1);
	if(*readWrite < 60) {
		*readWrite = 0;
	}
	else {
		*readWrite = 1;
	}
	
	//mem location
	*requestMem = (rand() % 32001);
};	

void buildSendRequest(msgQ *msg, long int sendMsgType, int readWrite, int requestMem, char requestType[], char memRequest[]) {
	msg->msg_type = sendMsgType;
	snprintf(msg->msg_txt, 11, "%ld", sendMsgType);
	strcat(msg->msg_txt, " ");
	snprintf(requestType, 5, "%d", readWrite);
	strcat(msg->msg_txt, requestType);
	strcat(msg->msg_txt, " ");
	snprintf(memRequest, 11, "%d", requestMem);
	strcat(msg->msg_txt, memRequest);
};

void sendTermRequest(msgQ *msg, long int sendMsgType) {
	snprintf(msg->msg_txt, 11, "%ld", sendMsgType);
	strcat(msg->msg_txt, " 100 0");
};

bool termCheck() {
	int randNum = ((rand() % 100) + 1);
	if(randNum <= 50) {
		return true;
	}
	else {
		return false;
	}
};
