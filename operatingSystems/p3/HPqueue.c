#include "functions.h"

static QNode *headHP, *tailHP;
//static pid_t theLPQueue[QUEUE_SIZE];

void InitHPQueue() {
	headHP = tailHP = NULL;
};

void ClearHPQueue() {
	QNode *temp;
	while(headHP != NULL) {
		temp = headHP;
		headHP = headHP->next;
		free(temp);
	}

	headHP = tailHP = NULL;
};

int EnqueueHP(PCB *pcbPtr, int arrPosition, FILE *fPtr) {
	QNode *temp;

	if(isHPQueueFull()) {
		return FALSE;
	}
	//create new node for the queue
	temp = (QNode *)malloc(sizeof(QNode));
	temp->sPtr = &pcbPtr[arrPosition];
	temp->next = NULL;

	//check for inserting first in the queue
	if(headHP == NULL) {
		headHP = tailHP = temp;
	}
	else {
		tailHP->next = temp; //insert into the queue
		tailHP = temp;       //set tail to new last node
	}
	fprintf(fPtr, "enqueuing process %d with PID %d\n", arrPosition, pcbPtr[arrPosition].pid);
	pcbPtr[arrPosition].enqueued += 1;
	return TRUE;
};

pid_t DequeueHP(PCB *pcbPtr, int *arrPosition, FILE *fPtr) {
	QNode *temp;
	if(isHPQueueEmpty()) {
		 return -1;
	}
	else {
		pcbPtr[*arrPosition] = headHP->sPtr[0];
		temp = headHP;
		headHP = headHP->next;
		free(temp);
		
		//check if queue is empty
		if(isHPQueueEmpty()) {
			headHP = tailHP = NULL;
		}
	}
	pcbPtr[*arrPosition].isSet = 0;
	pcbPtr[*arrPosition].inQueue = 0;
	pcbPtr[*arrPosition].flag = 1;
	pcbPtr[*arrPosition].dequeued += 1;
	fprintf(fPtr, "dequeuing process %d with PID %d\n", *arrPosition, pcbPtr[*arrPosition].pid);
	return pcbPtr[*arrPosition].pid;
};

int isHPQueueEmpty() {
	if(headHP == NULL) {
		return 1;
	}
	return 0;
};

int isHPQueueFull() {
	return FALSE;
};

