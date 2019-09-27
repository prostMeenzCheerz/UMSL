#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "header.h"

static QNode *head, *tail;

void InitQueue() {
	head = tail = NULL;
};

void Enqueue(userPCB *userPcbArrPtr, int arrPosition, unsigned int *seconds, unsigned int *milliseconds) {
	QNode *temp;

	//create new node for the queue
	temp = (QNode *)malloc(sizeof(QNode));
	temp->sPtr = &userPcbArrPtr[arrPosition];
	temp->next = NULL;

	//check for inserting first in the queue
	if(head == NULL) {
		head = tail = temp;
	}
	else {
		tail->next = temp; //insert into the queue
		tail = temp;       //set tail to new last node
	}
};

void Dequeue(userPCB *userPcbArrPtr, unsigned int *seconds, unsigned int *milliseconds, FILE **fPtr, int *lines) {
	QNode *temp;
	if(isQueueEmpty()) {
		 return;
	}
	else {
		temp = head;

		int pos = temp->sPtr->arrPosition;
		userPcbArrPtr[pos].timeDequeuedSeconds = *seconds;
		userPcbArrPtr[pos].timeDequeuedMilli = *milliseconds;
		userPcbArrPtr[pos].isBlocked = 0;
		head = head->next;
		free(temp);
		unsigned int secondsSpent = userPcbArrPtr[pos].timeDequeuedSeconds - userPcbArrPtr[pos].timeEnqueuedSeconds;
		unsigned int milliSpent = userPcbArrPtr[pos].timeDequeuedMilli - userPcbArrPtr[pos].timeEnqueuedMilli;
		fprintf(*fPtr, "%d: process P%d was woken up at %u.%u and spent %u.%u seconds in the queue\n", *lines, pos, *seconds, *milliseconds, secondsSpent, milliSpent);
		(*lines)++;
		//check if queue is empty
		if(isQueueEmpty()) {
			head = tail = NULL;
		}
	}
};

void searchQueue(userPCB *userPcbArrPtr, ossPCB *ossPtr, int *arrPosition, int resource, unsigned int *seconds, unsigned int *milliseconds, FILE **fPtr, int *lines, int *run) {
	QNode *temp;
	QNode *previous;

	int pos = 0;
	if(isQueueEmpty()) {
		return;
	}
	else {
		temp = head;
		temp->next = head->next;
		pos = temp->sPtr->arrPosition;
		if(bankersAlgorithm(userPcbArrPtr, ossPtr, pos)) {
			*run++;
			getResource(userPcbArrPtr, ossPtr, pos, resource);
			Dequeue(userPcbArrPtr, seconds, milliseconds, fPtr, lines);
			userPcbArrPtr[pos].timeDequeuedSeconds = *seconds;
			userPcbArrPtr[pos].timeDequeuedMilli = *milliseconds;
			userPcbArrPtr[pos].isBlocked = 0;
		}
		else {
			previous = temp;
			temp = head->next;
			if((void *)temp != NULL) {
				pos = temp->sPtr->arrPosition;
				if(bankersAlgorithm(userPcbArrPtr, ossPtr, pos)) {
					*run++;
					getResource(userPcbArrPtr, ossPtr, pos, resource);
					userPcbArrPtr[pos].timeDequeuedSeconds = *seconds;
					userPcbArrPtr[pos].timeDequeuedMilli = *milliseconds;
					userPcbArrPtr[pos].isBlocked = 0;
					previous->next = temp->next;
					free(temp);
				}
				previous = temp;
				temp = temp->next;
			}
		}
	}
};

int isQueueEmpty() {
	if(head == NULL) {
		return TRUE;
	}
	return FALSE;
};
