#include "functions.h"

static QNode *headLP, *tailLP;
//static pid_t theLPQueue[QUEUE_SIZE];

void InitLPQueue() {
	headLP = tailLP = NULL;
};

void ClearLPQueue() {
	QNode *temp;
	while(headLP != NULL) {
		temp = headLP;
		headLP = headLP->next;
		free(temp);
	}

	headLP = tailLP = NULL;
};

int EnqueueLP(PCB *pcbPtr, int arrPosition, FILE *fPtr) {
	QNode *temp;

	if(isLPQueueFull()) {
		return FALSE;
	}
	//create new node for the queue
	temp = (QNode *)malloc(sizeof(QNode));
	temp->sPtr = &pcbPtr[arrPosition];
	temp->next = NULL;

	//check for inserting first in the queue
	if(headLP == NULL) {
		headLP = tailLP = temp;
	}
	else {
		tailLP->next = temp; //insert into the queue
		tailLP = temp;       //set tail to new last node
	}
	fprintf(fPtr, "enqueueing process %d with PID %d\n", arrPosition, pcbPtr[arrPosition].pid);
	pcbPtr[arrPosition].enqueued += 1;
	return TRUE;
};

pid_t DequeueLP(PCB *pcbPtr, int *arrPosition, FILE *fPtr) {
	QNode *temp;
	if(isLPQueueEmpty()) {
		 return FALSE;
	}
	else {	
		*arrPosition = headLP->sPtr[0].position;
		pcbPtr[*arrPosition] = headLP->sPtr[0];
		temp = headLP;
		headLP = headLP->next;
		free(temp);
		
		//check if queue is empty
		if(isLPQueueEmpty()) {
			headLP = tailLP = NULL;
		}
	}
	pcbPtr[*arrPosition].dequeued += 1;
	pcbPtr[*arrPosition].inQueue = 0;
	pcbPtr[*arrPosition].isSet = 0;
	pcbPtr[*arrPosition].flag = 1;
	fprintf(fPtr, "dequed process %d with PID %d\n", *arrPosition, pcbPtr[*arrPosition].pid);	
	return pcbPtr[*arrPosition].pid;
};

int isLPQueueEmpty() {
	if(headLP == NULL) {
		return 1;
	}
	return 0;
};

int isLPQueueFull() {
	return FALSE;
};

