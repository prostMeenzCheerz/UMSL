#include <stdio.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "header.h"

volatile sig_atomic_t signalCalled = 0;
static void sigHandler(int signo);

int main(int argc, char *argv[]) {
	//group of variables used throughout the program
	//option for getopt, numProcesses used with getopt to get number of process, status is used for terminating child process, pos is position of pidArr that child is using, 
	//forkedYet is for checking whether or not I need to grab a random time to fork a child, children is to keep track of the number of processes that are currently running, 
	//request is the read/write request from the child, memLocation is where the child is requesting memory from, fTablePtr is the "queue" head used for second-chance clock algorithm
	//and frameTable is a 2D array keeping track of memory location, second chance bit, and dirty bit
	int i, j, option = 0, numProcesses = 0, status = 0, pos = 0, forkedYet = 0, children = 0, request= 0, page = 0, memLocation = 0, fTablePtr = 0, access = 1, frameTable[256][3] = {{0}};
	int times = 1, deaths = 0, pageFaults = 0, lines = 0;
	//keys used with the ftok function
	key_t msgKey = 0, memKey = 0, semKey = 0;

	//ID's for the shared memory locations
	int msgID = 0, memID = 0, semID = 0;

	//char arrays to hold the shared memory locations that will
	//be sent to the child process
	char msgMem[10] = {0}, memMem[10] = {0}, semMem[10] = {0}, string[100] ={0};

	//variables to be used with the logical clock
	unsigned int *seconds = 0, *nanoseconds = 0, forkSeconds = 0, forkNano = 0;

	//file pointer
	FILE *fPtr = NULL;

	//semaphore pointer
	sem_t *sem = 0;

	//pid variables
	pid_t pid = 0, childPid = 0, tempPid = 0;

	//signal handlers
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigHandler;

	sigaction(SIGINT, &act, 0);
	sigaction(SIGALRM, &act, 0);
	alarm(2);

	//struct to message queue
	//underneath is the kernel level struct to queue
	//it's used to check if the queue has any messages		
	msgQ msg = {.msg_type = 0, .msg_txt = {0}};	
	struct msqid_ds msqid_ds, *buf;
	buf = &msqid_ds;

	//get command line argument for total num processes at 1 time
	getOpt(argc, argv, &option, &numProcesses);

	//get pid array created and initialized
	pid_t pidArr[numProcesses];
	intializePidArr(pidArr, numProcesses);

	//initialize an array of pointers
	//I'm going to go ahead and malloc all the structures I need since 
	//I know how many processes I'm going to run at once
	PCB *pcbArr[numProcesses];
	createDataForPtrs(pcbArr, numProcesses);
	
	//create a pointer to array of pointers
	//and initialize the struct values to zero
	PCB *(*pcbPtr)[] = &pcbArr;
	initializePcbArr(pcbPtr, numProcesses);

	//setup shared memory and get everything prepared for shared memory
	getKeys(&msgKey, &memKey, &semKey);
	openSharedMem(&msgID, &memID, &semID, msgKey, memKey, semKey);
	pointToShMem(&seconds, &nanoseconds, &sem, memID, semID);
	initializeSemaphore(&sem);
	initializeFilePtr(&fPtr);
	createExecArgs(msgMem, memMem, semMem, msgID, memID, semID);

	//"critical section"
	while(signalCalled == 0) {
		incrementClock(seconds, nanoseconds);
		if((children < numProcesses) && (forkedYet == 0)) {
			getForkTime(seconds, nanoseconds, &forkSeconds, &forkNano);
			forkedYet++;
		}
		if((children < numProcesses) && ((*seconds > forkSeconds) || ((*seconds == forkSeconds) && (*nanoseconds >= forkNano)))) {
			getArrPosition(pidArr, &pos, numProcesses);
			childPid = forkChild(pid, pidArr, msgMem, memMem, semMem);
			pidInVector(pidArr, pos, childPid);
			(*pcbPtr)[pos]->userPid = childPid;
			(*pcbPtr)[pos]->processNum = pos;
			children++;	
			forkedYet--;
		}
		msgctl(msgID, IPC_STAT, buf);
		while((buf->msg_qnum != 0) && children == numProcesses) {
			msgrcv(msgID, &msg, sizeof(msg), 0, IPC_NOWAIT);
			strcpy(string, msg.msg_txt);
			getInfo(string, &tempPid, &request, &memLocation, &page, &times, &deaths);
			if(request == 100) {
				memset(string, 0, 100);
				msg.msg_type = tempPid * tempPid;
				msgsnd(msgID, &msg, sizeof(msg), IPC_NOWAIT);
				wait(&status);
				msg.msg_type = 0;
				incrementClock(seconds, nanoseconds);
				clearStructPtr(pcbPtr, &i, tempPid, numProcesses);
				pidArr[i] = 0;
				removeFromFTable(frameTable, tempPid);
				children--;
				fprintf(fPtr, "process %d is terminating at %u.%u\n", i, *seconds, *nanoseconds);
				msgctl(msgID, IPC_STAT, buf);
				continue;
			}	
			for(i = 0; i < numProcesses; i++) {
				if(tempPid == pidArr[i]) {
					handleRequest(&fPtr, pidArr, frameTable, tempPid, pcbPtr, i, &j, memLocation, request, &fTablePtr, seconds, nanoseconds, &access, &children, page, &pageFaults, &lines);
					break;
				}
			}
			incrementClock(seconds, nanoseconds);
			memset(string, 0, 100);
			msg.msg_type = tempPid * tempPid;
			msgsnd(msgID, &msg, sizeof(msg), IPC_NOWAIT);
			msgctl(msgID, IPC_STAT, buf);
			msg.msg_type = 0;
		}
	}

	if(signalCalled == 1) {
		printf("%d memory accesses per second\n", (access / *seconds));
		printf("%f page faults per memory access\n", ((float)pageFaults / (float)access));
		int time = pageFaults * 15000000;
		time += (access - pageFaults) * 10;
		int nano = (*seconds * 1000000000) + *nanoseconds;
		printf("%f is the average memory access speed in milliseconds\n", (float)time / (float)nano);
		unattachFromMem(seconds, sem);
		destroySharedMem(&memID, &msgID, &semID);
		fclose(fPtr);
		free(**pcbPtr);
		kill(0, SIGTERM);
	}
	//cleanup
	fclose(fPtr);
	free(**pcbPtr);
	while((pid = wait(&status)) > 0);
	unattachFromMem(seconds, sem);
	destroySharedMem(&memID, &msgID, &semID);
	return 0;
};

static void sigHandler(int signo) {
	signalCalled = 1;
};
