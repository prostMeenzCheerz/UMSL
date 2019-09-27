#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "functions.h"

volatile sig_atomic_t signalCalled = 0;
static void sigHandler(int signo);
int main(int argc, char *argv[]) {
	srand(time(NULL));	

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigHandler;

	int billion = 1000000000;
	int bitVector[18] = {0};
	key_t strctKey = 0, semKey = 0, timeKey = 0, qKey = 0;
	int strctMemId = 0, semMemId = 0, timeMemId = 0, qMemId = 0;

	unsigned int *seconds = 0, *nanoseconds = 0;
	int  *quantum = 0, forkTimeSecs = 0, forkTimeNano = 0, arrPosition = 0, terminatingPID = 0, forked = 0, forkedYet = 0, status = 0, dequeued = 0;
	PCB *pcbPtr = NULL;
	sem_t *sem = NULL;
	pid_t pid = 0;
	bool isFull = true, processRunning = false, processTerminating = false, isStopped = false;

	char sharedStrctMem[10] = {0}, sharedSemMem[10] = {0}, sharedClockMem[10] = {0}, childArrPosition[10] = {0}, timeQuantum[10] = {0};

	FILE *fPtr = fopen("log.txt", "w");
	if(fPtr == NULL) {
		fprintf(stderr, "error opening file...\n");
		exit(1);
	}
	freopen("log.txt", "a", fPtr);
	if(fPtr == NULL) {
		fprintf(stderr, "error reopening the file... \n");
	}

	createSharedMemKeys(&strctKey, &semKey, &timeKey, &qKey);
	createSharedMem(&strctMemId, &semMemId, &timeMemId, &qMemId, strctKey, semKey, timeKey, qKey);
	attachToSharedMem(&pcbPtr, &sem, &seconds, &nanoseconds, &quantum, strctMemId, semMemId, timeMemId, qMemId);
	initializeSemaphore(&sem);

	*quantum = 100000;

	InitLPQueue();
	InitHPQueue();
	
	sigaction(SIGINT, &act, 0);
	sigaction(SIGALRM, &act, 0);
	alarm(2);

	do {

		sem_wait(sem);
		isVectorFull(bitVector, &isFull);
		if(isFull == false) {
			if(forkedYet == 0) {
				getRandomForkTime(seconds, nanoseconds, &forkTimeSecs, &forkTimeNano);
				forkedYet++;
			}
			if((forked < 1) ||(forkTimeSecs > *seconds) || (forkTimeSecs >= *seconds && forkTimeNano >= *nanoseconds)) {
				forkChild(sharedStrctMem, sharedSemMem, sharedClockMem, childArrPosition, timeQuantum, &pcbPtr, &arrPosition, strctMemId, semMemId, timeMemId, qMemId, bitVector, pid);
				fprintf(fPtr, "forked a new process at position %d at %u.%u seconds\n", arrPosition, *seconds, *nanoseconds);
				forkedYet--;
				forked++;
				*nanoseconds += 1000000;
				if(*nanoseconds >= billion) {
					(*seconds)++;
					*nanoseconds = *nanoseconds - billion;
				}
			}	
		}

		enqueueReadyProcess(pcbPtr, fPtr);
		checkRunningProcess(pcbPtr, &processRunning);
		if(processRunning == false && dequeued == 0) {
			incrementTheClock(&seconds, &nanoseconds);
			fprintf(fPtr, "no processes running, so increment the clock, time is %u.%u seconds\n", *seconds, *nanoseconds);
			if(*nanoseconds >= billion) {
				(*seconds)++;
				*nanoseconds = *nanoseconds - billion;
			}
			if(isHPQueueEmpty() == 0) {
				DequeueHP(pcbPtr, &arrPosition, fPtr);
				dequeued++;
			}
			else if(isHPQueueEmpty() == 1 && isLPQueueEmpty() == 0) {
				DequeueLP(pcbPtr, &arrPosition, fPtr);
				dequeued++;
			}
			
		}
		checkForStoppedProcess(pcbPtr, &arrPosition, &dequeued, &isStopped);
		if(isStopped == true) {
			addUserCPUtimeToClock(pcbPtr, arrPosition, seconds, nanoseconds);
			fprintf(fPtr, "process %d has stopped, adding its time to the clock, time is %u.%u seconds\n", arrPosition, *seconds, *nanoseconds);
		}
		checkForTerminatingProcess(pcbPtr, &arrPosition, &terminatingPID, &processTerminating);
		if(processTerminating == true) {
			pcbPtr[arrPosition].msgReceived = 1;
			fprintf(fPtr, "process %d is terminating, its total time in the system was %u.%u\n", arrPosition, pcbPtr[arrPosition].totalTimeInSystem_seconds, pcbPtr[arrPosition].totalTimeInSystem_nano);
			if(waitpid(pcbPtr[arrPosition].pid, &status, WUNTRACED) == -1);
			decrementBitVector(bitVector);
			clearStruct(pcbPtr, arrPosition);
		}
	sem_post(sem);
	}while((forked < 100) || (signalCalled == 0));


	if(signalCalled == 1) {
		unattachFromMem(sem, pcbPtr, seconds, quantum);
		removeMemSegments(&strctMemId, &semMemId, &timeMemId, &qMemId);
		printf("terminating......\n");
		kill(0, SIGTERM);
	}


	fclose(fPtr);
	while((pid = wait(&status)) > 0);
	unattachFromMem(sem, pcbPtr, seconds, quantum);	
	removeMemSegments(&strctMemId, &semMemId, &timeMemId, &qMemId);

return 0;
};

static void sigHandler(int signo) {
	signalCalled = 1;
};
