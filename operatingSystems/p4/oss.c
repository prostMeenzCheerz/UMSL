#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

volatile sig_atomic_t signalCalled = 0;
static void sigHandler(int signo);

int main(int argc, char *argv[]) {

	srand(getpid());

	pid_t pid = 0, uPid = 0;
	key_t sharedClockKey = 0, msgQkey = 0, termKey = 0, userPcbKey = 0;
	int clockMemLocation = 0, msgID = 0, termID = 0, shPcbMem = 0, status = 0, forkYet = 0, resourceArrSize = 20, arrPosition = 0, bound = 4, bitVector[18] = {0}, vectorSize = 18, numProcess = 0, option = 0, 
		verbose = 1, lines = 0, request = 0, resource = 0, accepted = 0, one = 0, run = 0;
	unsigned int *seconds = 0, *milliseconds = 0, forkSeconds = 0, forkMilli = 0;
	float percentage = 0.0;
	char clockMem[10] = {0}, msgMem[10] = {0}, termMem[10] = {0}, uPcbMem[20] = {0}, maxBound[20] = {0};
	FILE *fPtr;

	ossPCB oss;
	ossPCB *ossPtr = &oss;
	userPCB *userPcbArrPtr = NULL;
	msg message = {.msg_type = 1, .msg_txt = {0}};
	term termMessage = {.term_type = 2, .term_txt = {0}};

	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigHandler;

	sigaction(SIGINT, &act, 0);
	sigaction(SIGALRM, &act, 0);
	alarm(2);

	InitQueue();
	getOpt(argc, argv, &option, &verbose);
	initializeFilePtr(&fPtr);
	createSharedMemKeys(&sharedClockKey, &msgQkey, &termKey, &userPcbKey);
	createSharedMem(&clockMemLocation, &msgID, &termID, &shPcbMem, sharedClockKey, msgQkey, termKey, userPcbKey);
	pointToSharedMem(&seconds, &milliseconds, &userPcbArrPtr, clockMemLocation, shPcbMem);
	makeResources(ossPtr, resourceArrSize);
	createExecArgs(clockMem, msgMem, termMem, uPcbMem, clockMemLocation, msgID, termID, shPcbMem);

	while(signalCalled == 0) {
		if(numProcess < 18) {
			if(forkYet == 0) {
				getForkTime(seconds, milliseconds, &forkSeconds, &forkMilli);
				forkYet++;
			}
			if((*seconds > forkSeconds) || ((*seconds == forkSeconds) && (*milliseconds >= forkMilli))) {
				getArrPosition(bitVector, vectorSize, &arrPosition);
				getOssMaxBound(ossPtr, userPcbArrPtr, arrPosition, bound, resourceArrSize, maxBound);
				forkChild(pid, clockMem, msgMem, termMem, uPcbMem, arrPosition, maxBound, ossPtr, resourceArrSize, bound, bitVector);
				numProcess++;
				forkYet--;
			}
		}
		incrementClock(seconds, milliseconds);
		if(msgrcv(termID, &termMessage, sizeof(termMessage), 2, IPC_NOWAIT) > 0) {
			uPid = atoi(strtok(termMessage.term_txt, " "));
			wait(&status);
			if(verbose == 0 && lines < 100000) {
				fprintf(fPtr, "%d: process P%d is terminating at %u.%u \n", lines, arrPosition, *seconds, *milliseconds);
				lines++;
			}
			resetTerminatingPcb(userPcbArrPtr, ossPtr, arrPosition);
			decrementVector(bitVector, arrPosition);
			numProcess--;
		}
		if(msgrcv(msgID, &message, sizeof(message), 1, IPC_NOWAIT) > 0) {
			char string[100] = {0};
			strcpy(string, message.msg_txt);
			arrPosition = atoi(strtok(string, " "));
			request = atoi(strtok(NULL, " "));
			if(request == 1) {
				one++;
				resource = atoi(strtok(NULL, " "));
				if(bankersAlgorithm(userPcbArrPtr, ossPtr, arrPosition)) {
					run++;
					getResource(userPcbArrPtr, ossPtr, arrPosition, resource);
					accepted++;
					if(verbose == 0 && lines < 100000) {
						fprintf(fPtr, "%d: process P%d requested resource %d at %u.%u\n", lines, arrPosition, resource, *seconds, *milliseconds);
						lines++;
					}
					if((accepted > 0) && ((accepted % 20) ==  0) &&(verbose == 0)) {
						int i, j, k;
						fprintf(fPtr, "\n");
						for(i = 0; i < 20; i++) {
							fprintf(fPtr, "\tR%d", i);
						}
						fprintf(fPtr, "\n");
						for(j = 0; j < 18; j++) {
							fprintf(fPtr, "P[%d]\t", j);
							for(k = 0; k < 20; k++) {
								fprintf(fPtr, "%2d\t", userPcbArrPtr[j].userResources[k]);
							}
							fprintf(fPtr, "\n");
						}
						fprintf(fPtr, "\n");
						lines += 21;
					}
				}
				else {
					Enqueue(userPcbArrPtr, arrPosition, seconds, milliseconds);
					userPcbArrPtr[arrPosition].timeEnqueuedSeconds = *seconds;
					userPcbArrPtr[arrPosition].timeEnqueuedMilli = *milliseconds;
					userPcbArrPtr[arrPosition].isBlocked = 1;
					fprintf(fPtr, "%d: process P%d was blocked at %u.%u\n", lines, arrPosition, *seconds, *milliseconds);
					lines++;
				}
			}
			else if(request == 2) {
				resource = atoi(strtok(NULL, " "));
				releaseResource(userPcbArrPtr, ossPtr, arrPosition, resource, verbose, &fPtr, &lines, seconds, milliseconds);
				searchQueue(userPcbArrPtr, ossPtr, &arrPosition, resource, seconds, milliseconds, &fPtr, &lines, &run);
			}
		}		
		incrementClock(seconds, milliseconds);
	}
	if(signalCalled == 1) {
		unattachFromMem(seconds, userPcbArrPtr);
		destroySharedMem(&clockMemLocation, &msgID, &termID, &shPcbMem);
		fclose(fPtr);

		percentage = one / accepted;
		printf("requests granted %d for a percentage of %.2f and the safety algorithm was run %d times\n", accepted, percentage, run);
		printf("time spent in queue will be shown in log file\n");

		kill(0, SIGTERM);
	}
	
	while((uPid = wait(&status)) > 0);
	unattachFromMem(seconds, userPcbArrPtr);
	destroySharedMem(&clockMemLocation, &msgID, &termID, &shPcbMem);
	fclose(fPtr);
	return 0;
};

static void sigHandler(int signo) {
	signalCalled = 1;
};
