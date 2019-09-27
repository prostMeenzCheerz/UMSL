#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>

#include "header.h"

void getOpt(int argc, char *argv[], int *option, int *verbose) {
	//switch statement to be used with the getopt function
	while ((*option = getopt(argc, argv, "v:")) != -1)
		switch(*option) {
		case 'v':
			*verbose  = strcmp(optarg, "on");
			break;
		case '?':
			break;
		default:
			break;
		}
};

void initializeFilePtr(FILE **fPtr) {
	*fPtr = fopen("log.txt", "w");
	if(fPtr == NULL) {
		fprintf(stderr, "error opening file...\n");
		exit(1);
	}
	freopen("log.txt", "a", *fPtr);
	if(fPtr == NULL) {
		fprintf(stderr, "error reopening the file... \n");
	}
};

void makeResources(ossPCB *ossPtr, int resourceArrSize) {
	int i;
	for(i = 0; i < resourceArrSize; i++) {
		int randNum = ((rand() % 10) + 1);
		ossPtr->mainResources[i] = randNum;
		ossPtr->resourcesRemaining[i] = randNum;
	}
};

void createExecArgs(char clockMem[], char msgMem[], char termMem[], char uPcbMem[], int clockMemLocation, int msgID, int termID, int shPcbMem) {
	snprintf(clockMem, 11, "%d", clockMemLocation);
	snprintf(msgMem, 11, "%d", msgID);
	snprintf(termMem, 11, "%d", termID);
	snprintf(uPcbMem, 21, "%d", shPcbMem);
};

void getForkTime(unsigned int *seconds, unsigned int *milliseconds, unsigned int *forkSeconds, unsigned int *forkMilli) {
	*forkSeconds = seconds[0];
	*forkMilli = milliseconds[0];

	int randMilli = ((rand() % 500) + 1);
	*forkMilli += randMilli;

	if(*forkMilli > 1000) {
		(*forkSeconds)++;
		*forkMilli = *forkMilli - 1000;
	}
	else if(*forkMilli == 1000) {
		(*forkSeconds)++;
		*forkMilli = 0;
	}
};

void getArrPosition(int bitVector[], int vectorSize, int *arrPosition) {
	int i;
	for(i = 0; i < vectorSize; i++) {
		if(bitVector[i] == 0) {
			*arrPosition = i;
			return;
		}
	}
};		

void getOssMaxBound(ossPCB *ossPtr, userPCB *userPcbArrPtr, int arrPosition, int bound, int resourceArrSize, char maxBound[]) {
	int i;
	for(i = 0; i < resourceArrSize; i++) {
		int randNum = ((rand() % bound) + 1);
		if(ossPtr->mainResources[i] == 1) {
			userPcbArrPtr[arrPosition].uResourcesMaxBounds[i] = 1;
		}
		else if(randNum > ossPtr->mainResources[i]) {
			userPcbArrPtr[arrPosition].uResourcesMaxBounds[i] = ossPtr->mainResources[i];
		}
		else {
			userPcbArrPtr[arrPosition].uResourcesMaxBounds[i] = randNum;
		}
	}
};

void incrementVector(int bitVector[], int arrPosition) {
	bitVector[arrPosition] = 1;
};

void decrementVector(int bitVector[], int arrPosition) {
	bitVector[arrPosition] = 0;
};

void forkChild(pid_t pid, char clockMem[], char msgMem[], char termMem[], char uPcbMem[], int arrPosition, char maxBound[], ossPCB *ossPtr, int resourceArrSize, int bound, int bitVector[]) {
	char arrPos[10] = {0};
	snprintf(arrPos, 11, "%d", arrPosition);

	if((pid = fork()) == -1) {
		fprintf(stderr, "cannot fork...\n");
		exit(1);
	}
	else if(pid == 0) {
		execlp("./user", "./user", clockMem, msgMem, termMem, uPcbMem, arrPos, maxBound, (char *)NULL);
	}
	
	incrementVector(bitVector, arrPosition);
};

void incrementClock(unsigned int *seconds, unsigned int *milliseconds) {
	*milliseconds += (rand() % (251 - 1) + 1);
	if(*milliseconds > 1000) {
		(*seconds)++;
		*milliseconds = *milliseconds - 1000;
	}
	else if(*milliseconds == 1000) {
		(*seconds)++;
		*milliseconds = 0;
	}
};

void resetTerminatingPcb(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition) {
	userPcbArrPtr[arrPosition].userPid = 0;
	userPcbArrPtr[arrPosition].arrPosition = 0;
	userPcbArrPtr[arrPosition].resourceRequested = 0;
	userPcbArrPtr[arrPosition].actionTaken = 0;
	userPcbArrPtr[arrPosition].isTerminating = 0;

	releaseAllResources(userPcbArrPtr, ossPtr, arrPosition);
	memset(userPcbArrPtr[arrPosition].userResources, 0, (sizeof(int) * 20));

	memset(userPcbArrPtr[arrPosition].uResourcesMaxBounds, 0, (sizeof(int) * 20));
	userPcbArrPtr[arrPosition].timeEnqueuedSeconds = 0;
	userPcbArrPtr[arrPosition].timeEnqueuedMilli = 0;
	userPcbArrPtr[arrPosition].timeDequeuedSeconds = 0;
	userPcbArrPtr[arrPosition].timeDequeuedMilli = 0;
};

void releaseAllResources(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition) {
	int i;
	for(i = 0; i < 20; i++) {
		ossPtr->resourcesRemaining[i] += userPcbArrPtr[arrPosition].userResources[i];
		userPcbArrPtr[arrPosition].userResources[i] = 0;
	}
};

void getResource(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition, int resource) {
	userPcbArrPtr[arrPosition].userResources[resource]++;
	ossPtr->resourcesRemaining[resource]--;
};

void releaseResource(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition, int resource, int verbose, FILE **fPtr, int *lines, unsigned int *seconds, unsigned int *milliseconds) {
	if((userPcbArrPtr[arrPosition].userResources[resource]) == 0) {
		return;
	}
	else {
		userPcbArrPtr[arrPosition].userResources[resource]--;
		ossPtr->resourcesRemaining[resource]++;
		if(verbose == 0 && *lines < 100000) {
			fprintf(*fPtr, "%d: process P%d is releasing resource %d at %u.%u\n", *lines,  arrPosition, resource, *seconds, *milliseconds);
			(*lines)++;
		}
	}
};

bool bankersAlgorithm(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition) {
	int i, need = 0;
	for(i = 0; i < 20; i++) {
		need = ((userPcbArrPtr[arrPosition].uResourcesMaxBounds[i]) - (userPcbArrPtr[arrPosition].userResources[i]));
		if(need > ossPtr->resourcesRemaining[i]) {
			return false;
		}
	}
	return true;
};

void outputTable(FILE **fPtr, userPCB *userPcbArrPtr) {
	int i, j, k;
	fprintf(*fPtr, "\n");
	for(i = 0; i < 20; i++) {
		fprintf(*fPtr, "\tR%d", i);
	}
	fprintf(*fPtr, "\n");
	for(j = 0; j < 18; j++) {
		fprintf(*fPtr, "P[%d]\t", j);
		for(k = 0; k < 20; k++) {
			fprintf(*fPtr, "%2d\t", userPcbArrPtr[j].userResources[k]);
		}
		fprintf(*fPtr, "\n");
	}
	fprintf(*fPtr, "\n");
};
