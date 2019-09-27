#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/msg.h>

#include "header.h"

void getOpt(int argc, char *argv[], int *option, int *numProcesses) {
	//switch statement to be used with the getopt function
	//to get number of process to run at once
	while ((*option = getopt(argc, argv, "x:")) != -1)
		switch(*option) {
		case 'x':
			*numProcesses = atoi(optarg);
			if(*numProcesses > 18) {
				*numProcesses = 18;
			}
			return;
		case '?':
			break;
		default:
			break;
		}
	
	*numProcesses = 18;
	return;
};

void getKeys(key_t *msgKey, key_t *memKey, key_t *semKey){
	//make the keys for shared memory creation
	*msgKey = ftok(".", 11);
	*memKey = ftok(".", 32);
	*semKey = ftok(".", 55);
};

void openSharedMem(int *msgID, int *memID, int *semID, key_t msgKey, key_t memKey, key_t semKey) {
	//get the ID's for the shared memory location
	*msgID = msgget(msgKey, 0777 | IPC_CREAT);
	*memID = shmget(memKey, SH_CLOCK_SZ, 0777 | IPC_CREAT);
	*semID = shmget(semKey, SEM_MEM_SZ, 0777 | IPC_CREAT);
};

void initializeSemaphore(sem_t **sem) {
	//initialize the semaphore
	sem_init(*sem, 1, 1);
};

void initializeFilePtr(FILE **fPtr) {
	//create the file pointer and open file stream
	//using freopen in conjunction with open so the user
	//does not have to any existing log files if the 
	//program is to be run more than once
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

void getArrPosition(pid_t pidArr[], int *pos, int numProcesses) {
	//have an array of child pids to keep track of who is who
	//and also used for sending messages back to user
	int i;
	for(i = 0; i < numProcesses; i++) {
		if(pidArr[i] == 0) {
			*pos = i;
			break;
		}
	}
};

void pidInVector(pid_t pidArr[], int pos, pid_t childPid) {
	//puts the child pid int the vector at specific location
	//both position and pid are used to help catalogue which
	//process is which
	pidArr[pos] = childPid;
};

pid_t forkChild(pid_t pid, pid_t pidArr[], char msgMem[], char memMem[], char semMem[]) {
	//fork the child process, and return it's pid to OSS
	if((pid = fork()) == -1) {
		fprintf(stderr, "cannot fork...\n");
		exit(1);
	}
	else if(pid == 0) {
		execlp("./process", "./process", msgMem, memMem, semMem, (char *)NULL);
	}
	return pid;
};

void destroySharedMem(int *memID, int *msgID, int *semID) {
	//destroy the shared memory locations
	shmctl(*memID, IPC_RMID, 0);
	msgctl(*msgID, IPC_RMID, 0);
	shmctl(*semID, IPC_RMID, 0);
};

void createExecArgs(char msgMem[], char memMem[], char semMem[], int msgID, int memID, int semID) {
	//create the exec args to be sent to the child process
	snprintf(msgMem, 11, "%d", msgID);
	snprintf(memMem, 11, "%d", memID);
	snprintf(semMem, 11, "%d", semID);
};

void incrementClockNoPageFault(unsigned int *seconds, unsigned int *nanoseconds) {
	*nanoseconds += 10;
	if(*nanoseconds > 1000000000) {
		(*seconds)++;
		*nanoseconds = nanoseconds[0] - 1000000000;
	}
	if(*nanoseconds == 1000000000) {
		(*seconds)++;
		*nanoseconds = 0;
	}
};

void incrementClockPageFault(unsigned int *seconds, unsigned int *nanoseconds) {
	*nanoseconds += 15000000;
	if(*nanoseconds > 1000000000) {
		(*seconds)++;
		*nanoseconds = nanoseconds[0] - 1000000000;
	}
	if(*nanoseconds == 1000000000) {
		(*seconds)++;
		*nanoseconds = 0;
	}
};

void incrementClock(unsigned int *seconds, unsigned int *nanoseconds) {
	*nanoseconds += 70000000;
	if(*nanoseconds > 1000000000) {
		(*seconds)++;
		*nanoseconds = nanoseconds[0] - 1000000000;
	}
	if(*nanoseconds == 1000000000) {
		(*seconds)++;
		*nanoseconds = 0;
	}
};

void getForkTime(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *forkSeconds, unsigned int *forkNano) {
	//get the time needed for OSS to fork a child
	//do a little math to keep the clock correct
	*forkSeconds = seconds[0];
	*forkNano = nanoseconds[0];

	int randNano = ((rand() % 500000) + 1);
	*forkNano += randNano;

	if(*forkNano > 1000000000) {
		(*forkSeconds)++;
		*forkNano = *forkNano - 1000000000;
	}
	else if(*forkNano == 1000000000) {
		(*forkSeconds)++;
		*forkNano = 0;
	}
};

void intializePidArr(pid_t pidArr[], int numProcesses) {
	//initalize all elements of pid array to zero
	int i;
	for(i = 0; i < numProcesses; i++) {
		pidArr[i] = 0;
	}
};

PCB** createDataForPtrs(PCB *pcbArr[], int numProcesses) {
	//allocate memory locations for the array of pointers
	//using malloc, and return the array
	int i;
	for(i = 0; i < numProcesses; i++) {
		pcbArr[i] = (PCB *)malloc(sizeof(PCB));
	}
	return pcbArr;
};

PCB* initializePcbArr(PCB *(*pcbPtr)[], int numProcesses) {
	//initialize the array of pointers by making all of the members
	//that it points to either zero for the userPid and processNum
	//and all the elements of the page table to -1
	int i, j;
	long unsigned int size = (sizeof((*pcbPtr)[i]->pageTable) / sizeof((*pcbPtr)[i]->pageTable[0]));
	for(i = 0; i < numProcesses; i++) {
		(*pcbPtr)[i]->userPid = 0;
		(*pcbPtr)[i]->processNum = 0;
		for(j = 0; j < size; j++) {
			(*pcbPtr)[i]->pageTable[j] = -1;
		}
	}
	return **pcbPtr;
};

void clearStructPtr(PCB *(*pcbPtr)[], int *i, pid_t tempPid, int numProcesses) {
	int j;
	for(j = 0; j < numProcesses; j++) {
		if((*pcbPtr)[j]->userPid == tempPid) {
			*i = j;
			break;
		}
	}
	(*pcbPtr)[*i]->userPid = 0;
	(*pcbPtr)[*i]->processNum = 0;
	for(j = 0; j < 32; j++) {
		(*pcbPtr)[*i]->pageTable[j] = -1;
	}
};

void getInfo(char string[], pid_t *tempPid, int *request, int *memLocation, int *page, int *times, int *deaths) {
	*tempPid = atoi(strtok(string, " "));
	*request = atoi(strtok(NULL, " "));
	if(*request == 100) {
		(*deaths)++;
		return;
	}
	*memLocation = atoi(strtok(NULL, " "));
	if(*memLocation < 1000) {
		*page = 0;
	}
	else {
		*page = floor(*memLocation / 1000);
	}
};

void removeFromFTable(int frameTable[][3], pid_t tempPid) {
	int i;
	for(i = 0; i < 256; i++) {
		if(frameTable[i][0] == tempPid) {
			frameTable[i][0] = 0;
		}
	}
};

void printTable(FILE **fPtr, int *access, int frameTable[][3]) {

	fprintf(*fPtr, "\n");
	int x = 0, m = 0, beginning = 1, end = 33;
	for(x = beginning; x < end; x++) {
		if(frameTable[x][0] == 0) {
			fprintf(*fPtr, ".");
			(*access)++;
		}
		else if(frameTable[x][0] > 0 && frameTable[x][1] == 0 && frameTable[x][2] == 0) {
			fprintf(*fPtr, "U");
			(*access)++;
		}
		else if(frameTable[x][0] > 0 && frameTable[x][2] == 1) {
			fprintf(*fPtr, "D");
			(*access)++;
		}
		if(x == (end - 1)) {
			fprintf(*fPtr, "\n");
			(*access)++;
		}
		if(x == (end - 1)) {
			for(m = beginning; m < end; m++) {
				if(frameTable[m][0] == 0) {
					fprintf(*fPtr, ".");
					(*access)++;
				}
				else if(frameTable[m][0] > 0 && frameTable[m][1] ==0 ) {
					fprintf(*fPtr, "0");
					(*access)++;
				}
				else if(frameTable[m][0] > 0 && frameTable[m][1] == 1) {
					fprintf(*fPtr, "1");
					(*access)++;
				}
				if(m == (end -1 )) {
					fprintf(*fPtr, "\n");
					(*access)++;
					beginning += 32;
					end += 32;
					if(end > 257) {
						fprintf(*fPtr, "\n");
						(*access)++;
						return;
					}
					break;	
				}
			}
		}
	}
};
void handleRequest(FILE **fPtr, pid_t pidArr[], int frameTable[][3], pid_t tempPid, PCB *(*pcbPtr)[], int i, int *j, int memLocation, int request, int *fTablePtr, 
	unsigned int *seconds, unsigned int *nanoseconds, int *access, int *children, int page, int *pageFaults, int *lines) {

//	fprintf(*fPtr, "process %d request %d memory %d page %d\n", i, request, memLocation, page);

	//first, check to see if the request is already on the frame table
	//if it is, flip the reference bit, and handle dirty bit accordingly
	for(*j = 0; *j < 256; (*j)++) {
		if((*pcbPtr)[i]->pageTable[page] == *j) {
			incrementClockNoPageFault(seconds, nanoseconds);
//			printf("%d %d %d\n", page, *j, (*pcbPtr)[i]->pageTable[page]);
			frameTable[*j][1] = 1;
			if(request == 1) {
				frameTable[*j][2] = 1;
				if(*access < 100000) {
					fprintf(*fPtr, "line:%d process %d is requesting write to memory location %d at %u.%u\n", *lines, i, memLocation, *seconds, *nanoseconds);
					(*access)++;
					(*lines)++;
					if(*access % 100 == 0 && *access < 100000) {
						printTable(fPtr, access, frameTable);
						(*access)++;
					}
					fprintf(*fPtr, "line:%d request has been granted as memory location is on frame %d\n", *lines, (*pcbPtr)[i]->pageTable[page]);
					(*access)++;
					(*lines)++;
					if(*access % 100 == 0 && *access < 100000) {
						printTable(fPtr, access, frameTable);
						(*access)++;
					}
					return;
				}
			}
			if(*access < 1000000) {
				fprintf(*fPtr, "line:%d process %d is requesting read of memory location %d at %u.%u\n", *lines, i, memLocation, *seconds, *nanoseconds);
				(*access)++;
				(*lines)++;
				if(*access % 100 == 0 && *access < 100000) {
					printTable(fPtr, access, frameTable);
					(*access)++;
				}
				fprintf(*fPtr, "line:%d request has been granted as memory location is on frame %d\n", *lines, (*pcbPtr)[i]->pageTable[page]);
				(*access)++;
				(*lines)++;
				if(*access % 100 == 0 && *access < 100000) {
					printTable(fPtr, access, frameTable);
					(*access)++;
				}
			}
		return;
		}
	}

	//if the request is not on the frame table, check for a section of the frame table that isn't holding a memory location
	//if we find one, add the pid  to the table, and the frame number to the page table, handle the dirty bit accordingly
	//we also update the frame table pointer to point to the following frame on the table to be used with the second-chance algorithm
	for(*j = 0; *j < 256; (*j)++) {
		if(frameTable[*j][0] == 0) {
			(*pageFaults)++;
			frameTable[*j][0] = tempPid;
			(*pcbPtr)[i]->pageTable[page] = *j; 
			*fTablePtr = (*j) + 1;
			if(*fTablePtr == 256) {
				*fTablePtr = 0;
			}
			if(request == 1) {
				frameTable[*j][2] = 1;
				if(*access < 100000) {
					fprintf(*fPtr, "line:%d process %d requesting write of memory location %d at %u.%u\n", *lines, i, memLocation, *seconds, *nanoseconds);
					(*access)++;
					(*lines)++;
					if(*access % 100 == 0 && *access < 100000) {
						printTable(fPtr, access, frameTable);
						(*access)++;
					}
					fprintf(*fPtr, "line:%d frame location %d was emtpy, storing process %d page number %d\n", *lines, *j, i, page);
					(*access)++;
					(*lines)++;
					incrementClockNoPageFault(seconds, nanoseconds);
					if(*access % 100 == 0 && *access < 100000) {
						printTable(fPtr, access, frameTable);
						(*access)++;
					}
				}
				else {
					incrementClockNoPageFault(seconds, nanoseconds);
				}
				return;
			}
			if(*access < 100000) {
				fprintf(*fPtr, "line:%d process %d requesting read of memory location %d at %u.%u\n", *lines, i, memLocation, *seconds, *nanoseconds);
				(*access)++;
				(*lines)++;
				if(*access % 100 == 0 && *access < 100000) {
					printTable(fPtr, access, frameTable);
					(*access)++;
				}
				fprintf(*fPtr, "line:%d frame location %d was emtpy, storing process %d page number %d\n", *lines, *j, i, page);
				(*access)++;
				(*lines)++;
				incrementClockNoPageFault(seconds, nanoseconds);
				if(*access % 100 == 0 && *access < 100000) {
					printTable(fPtr, access, frameTable);
					(*access)++;
				}
			}
			else {
				incrementClockPageFault(seconds, nanoseconds);
			}
			return;
		}
	}

	//finally, if the previous conditionals fail, we use the second-chance clock algorithm
	//to replace the frame, starting at the frameTable position that the
	//frame table pointer variable is pointing to
	//it will replace the frame at the first second-chance bit 0 it comes across
	//otherwise, it will flip the bit to zero and move on
	int startingValue = *fTablePtr, limit = 256;
	for(*j = startingValue; *j < limit; (*j)++) {
		if(frameTable[*j][1] == 0) {
			(*pageFaults)++;
			frameTable[*j][0] = tempPid;
			(*pcbPtr)[i]->pageTable[page] = *j;
			*fTablePtr = (*j)++;
			if(frameTable[*j][2] == 1) {
				if(*access < 100000) {
					fprintf(*fPtr, "line:%d process %d requesting write of memory location %d at %u.%u\n", *lines, i, memLocation, *seconds, *nanoseconds);
					(*access)++;
					(*lines)++;
					if(*access % 100 == 0 && *access < 100000) {
						printTable(fPtr, access, frameTable);
						(*access)++;
					}
					incrementClockPageFault(seconds, nanoseconds);
					fprintf(*fPtr, "line:%d memory location %d is not on frame table, PAGE FAULT, incrementing clock to %u.%u\n", *lines, memLocation, *seconds, *nanoseconds);
					(*access)++;
					(*lines)++;
					if(*access % 100 == 0 && *access < 100000) {
						printTable(fPtr, access, frameTable);
						(*access)++;
					}
				}
				else {
					if(*access < 10000) {
						fprintf(*fPtr, "line:%d process %d requesting read of memory location %d at %u.%u\n", *lines, i, memLocation, *seconds, *nanoseconds);
						(*access)++;
						if(*access % 100 == 0 && *access < 100000) {
							printTable(fPtr, access, frameTable);
							(*access)++;
							(*lines)++;
						}
						incrementClockPageFault(seconds, nanoseconds);
						fprintf(*fPtr, "line:%d memory location %d is not on the frame table, PAGE FAULT, incremeting clock to %u.%u\n", *lines, memLocation, *seconds, *nanoseconds);
						(*access)++;
						(*lines)++;
						if(*access % 100 == 0 && *access < 100000) {
							printTable(fPtr, access, frameTable);
							(*access)++;
						}
					}
				}
				return;
			}
			if(request == 1) {
				frameTable[*j][2] = 1;
			}
			return;
		}
		else {
			frameTable[*j][1] = 0;
		}
		if((startingValue > 0) && (*j == 255)) {
			limit = startingValue;
			startingValue = 0;
		}
	}
};
