#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "header.h"

void init_user(userPCB *userPcbArrPtr, int pos) {
	userPcbArrPtr[pos].arrPosition = pos;
	userPcbArrPtr[pos].userPid = getpid();
};

int action(userPCB *userPcbArrPtr, int pos) {
	int val;
	int randNum = (rand() % 100);
	if(randNum < 5) {
		//terminate
		val = 0;
	}
	else if((randNum > 4 && randNum < 55 )) {
		//request
		val = 1;
	}
	else if(randNum > 54) {
		//release
		val = 2;
	}
	
	return val;
};

void getRequestedResource(userPCB *userPcbArrPtr, int pos, int *randomResource) {
	*randomResource = (rand() % 20);
	if(userPcbArrPtr[pos].userResources[*randomResource] >= userPcbArrPtr[pos].uResourcesMaxBounds[*randomResource]) {
		getRequestedResource(userPcbArrPtr, pos, randomResource);
	}
};

void getReleasedResource(userPCB *userPcbArrPtr, int pos, int *randomResource) {
	*randomResource = (rand () % 20);
	if(userPcbArrPtr[pos].uResourcesMaxBounds[*randomResource] == 0) {
		getReleasedResource(userPcbArrPtr, pos, randomResource);
	}
};

void pickATime(unsigned int *seconds, unsigned int *milliseconds, unsigned int *actionSeconds, unsigned int *actionMilli) {
	int randS = (rand() % 5);
	int randM = (rand() % 501);

	*actionMilli = milliseconds[0] + randM;
	*actionSeconds = seconds[0] + randS;
	if(*actionMilli > 1000) {
		(*actionSeconds)++;
		*actionMilli = *actionMilli - 1000;
	}
};
