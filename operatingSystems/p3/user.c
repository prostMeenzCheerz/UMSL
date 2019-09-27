#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>

#include "functions.h"

int main(int argc, char *argv[]) {
	srand(getpid());
	
	int strctMemId = atoi(argv[1]);
	int semMemId = atoi(argv[2]);
	int timeMemId = atoi(argv[3]);
	int pcbPosition = atoi(argv[4]);
	int qMemId = atoi(argv[5]);

	unsigned int *seconds = 0, *nanoseconds = 0;
	PCB *pcbPtr = NULL;
	sem_t *sem = NULL;
	int *quantum = NULL;
	int localQuantum = 0;
	int totalQuantumUsed = 0;

	unsigned int tempTimeInSystem_seconds = 0, tempTimeInSystem_nano = 0, tempTimeSinceLastBurst_seconds = 0, tempTimeSinceLastBurst_nano = 0, waitToQueue_seconds = 0, waitToQueue_nano = 0;

	attachToSharedMem(&pcbPtr, &sem, &seconds, &nanoseconds, &quantum, strctMemId, semMemId, timeMemId, qMemId);
	setQuantum(pcbPtr, pcbPosition, quantum, &localQuantum, sem);
	//set the struct member position to its array position and initialize some parts of the struct

	initUser(pcbPtr, pcbPosition, &seconds, &nanoseconds, &tempTimeInSystem_seconds, &tempTimeInSystem_nano, sem);

	do {
		sem_wait(sem);	
		if(pcbPtr[pcbPosition].flag == 1) {
			pcbPtr[pcbPosition].isRunning = 1;
			getAction(&pcbPtr, pcbPosition, &seconds, &nanoseconds, localQuantum, &waitToQueue_seconds, &waitToQueue_nano, &tempTimeInSystem_seconds, &tempTimeInSystem_nano, &tempTimeSinceLastBurst_seconds, 
				&tempTimeSinceLastBurst_nano, &totalQuantumUsed, sem);
		}
		if(waitToQueue_nano != 0) {
			if((*seconds >= waitToQueue_seconds && *nanoseconds >= waitToQueue_nano)) {
				pcbPtr[pcbPosition].isSet = 1;
				waitToQueue_nano = 0;
				waitToQueue_seconds = 0;
			}
		}
		if((pcbPtr[pcbPosition].totalTimeInSystem_seconds > 0) || (pcbPtr[pcbPosition].totalTimeInSystem_seconds == 0 && pcbPtr[pcbPosition].totalTimeInSystem_nano >= 50000000)) {
			int randNum = rand() % 1;
			if(randNum == 0) {	
				pcbPtr[pcbPosition].isTerminating = 1;
			}
		}
		sem_post(sem);
	}while(pcbPtr[pcbPosition].isTerminating == 0);

	getUserTotalSystemTime(pcbPtr, pcbPosition, seconds, nanoseconds, &tempTimeInSystem_seconds, &tempTimeInSystem_nano, sem);
	while(pcbPtr[pcbPosition].msgReceived == 0);
	unattachFromMem(sem, pcbPtr, seconds, quantum);	

return 0;
};
