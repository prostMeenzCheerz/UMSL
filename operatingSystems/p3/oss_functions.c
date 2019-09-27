#include "functions.h"

void createExecArgs(char *sharedStrctMem, char *sharedSemMem, char *sharedClockMem, char *childArrayPosition, char *timeQuantum, int strctMemId, int semMemId, int timeMemId, int qMemId, int arrPosition) {
	snprintf(sharedStrctMem, sizeof(sharedStrctMem) + 2, "%d", strctMemId);
	snprintf(sharedSemMem, sizeof(sharedSemMem) + 2, "%d", semMemId);
	snprintf(sharedClockMem, sizeof(sharedClockMem) + 2, "%d", timeMemId);
	snprintf(childArrayPosition, sizeof(childArrayPosition) + 2, "%d", arrPosition);
	snprintf(timeQuantum, sizeof(timeQuantum) + 2, "%d", qMemId);
};

void getArrayPosition(PCB *pcbPtr, int *arrPosition) {
	int i;
	for(i = 0; i < 18; i++) {
		if(pcbPtr[i].isSet == 0) {
			*arrPosition = i;
			break;
		}
	}
};

void updateVector(int bitVector[]) {
	int i;
	for(i = 0; i < 18; i++) {
		if(bitVector[i] == 0) {
			bitVector[i] = 1;
			return;
		}
	}
};

bool isVectorFull(int bitVector[], bool *isFull) {
	int i;
	for(i = 0; i < 18; i++) {
		if(bitVector[i] == 0) {
			*isFull = false;
			return *isFull;
		}
	}
	*isFull = false;
	return *isFull;
}; 

bool isVectorEmpty(int bitVector[], bool *hasValue) {
	int i;
	for(i = 0; i < 18; i++) {
		if(bitVector[i] == 1) {
			*hasValue = true;
			return *hasValue;
		}
	}
	*hasValue = false;
	return *hasValue;
};

void setThePriority(PCB *pcbPtr, int arrPosition) {
	int priority = rand() % 10;
	if(priority  == 0) {
		pcbPtr[arrPosition].processPriority = 0;
	}
	else {
		pcbPtr[arrPosition].processPriority = 1;
	}
};

void forkChild(char *sharedStrctMem, char *sharedSemMem, char *sharedClockMem, char *childArrayPosition, char *timeQuantum, PCB **pcbPtr, int *arrPosition, 
	int strctMemId, int semMemId, int timeMemId, int qMemId, int bitVector[], pid_t pid) {
	getArrayPosition(*pcbPtr, arrPosition);
	setThePriority(*pcbPtr, *arrPosition);
	createExecArgs(sharedStrctMem, sharedSemMem, sharedClockMem, childArrayPosition, timeQuantum, strctMemId, semMemId, timeMemId, qMemId, *arrPosition);

	if((pid = fork()) == -1) {
		fprintf(stderr, "failed to fork! ...\n");
		exit(1);
	}
	else if(pid == 0) {
		execlp("./user", "./user", sharedStrctMem, sharedSemMem, sharedClockMem, childArrayPosition, timeQuantum, (char *)NULL);
	}
	updateVector(bitVector);
	
};

void getRandomForkTime(unsigned int *seconds, unsigned int *nanoseconds, int *forkTimeSecs, int *forkTimeNano) {
		*forkTimeSecs = 0;
		*forkTimeNano = 0;
		int randNum = (rand() % 3);

		*forkTimeSecs = *seconds;
		*forkTimeNano = *nanoseconds;

		*forkTimeSecs += randNum;
		
};

void incrementTheClock(unsigned int **seconds, unsigned int **nanoseconds) {
		(**seconds)++;
		**nanoseconds += (rand() % 1001);
};

void removeMemSegments(int *strctMemId, int *semMemId, int *timeMemId, int *qMemId) {
	shmctl(*strctMemId, IPC_RMID, NULL);
	shmctl(*semMemId, IPC_RMID, NULL);
	shmctl(*timeMemId, IPC_RMID, NULL);
	shmctl(*qMemId, IPC_RMID, NULL);
};

bool checkRunningProcess(PCB *pcbPtr, bool *processRunning) {
	int i;
	for(i = 0; i < 18; i++) {
		if(pcbPtr[i].isRunning == 1) {
			*processRunning = true;
			return *processRunning;
		}
	}
	return *processRunning;
};

void enqueueReadyProcess(PCB *pcbPtr, FILE *fPtr) {
	int i;
	for(i = 0; i < 18; i++) {
		if(pcbPtr[i].isSet == 1) {
			if(pcbPtr[i].processPriority == 0) {
				EnqueueHP(pcbPtr, i, fPtr);			
			}
			else {
				EnqueueLP(pcbPtr, i, fPtr);
			}
		}
	}
};
bool checkForStoppedProcess(PCB *pcbPtr, int *arrPosition, int *deqeued, bool *isStopped) {
	int i;
	for(i = 0; i < 18; i++) {
		if(pcbPtr[i].isRunning == -1) {
			*arrPosition = i;
			*isStopped = true;
			return *isStopped;
		}
	}
	*isStopped = false;
	return *isStopped;
};

void checkForTerminatingProcess(PCB *pcbPtr, int *arrPosition, int *terminatingPID, bool *processTerminating) {
	int i;
	for(i = 0; i < 18; i++) {
		if(pcbPtr[i].isTerminating == 1) {
			*processTerminating = true;
			*arrPosition = i;
			*terminatingPID = pcbPtr[*arrPosition].pid;
			return;
		}
	}
};

void decrementBitVector(int bitVector[]) {
	int i;
	for(i = 18; i > 0; i--) {
		if(bitVector[i] == 1) {
			bitVector[i] = 0;
			break;
		}
	}
};

void clearStruct(PCB *pcbPtr, int arrPosition) {
	pcbPtr[arrPosition].pid = 0;
	pcbPtr[arrPosition].position = 0;
	pcbPtr[arrPosition].processPriority = 0;
	pcbPtr[arrPosition].isSet = 0;
	pcbPtr[arrPosition].inQueue = 0;
	pcbPtr[arrPosition].flag = 0;
	pcbPtr[arrPosition].totalTimeInSystem_seconds = 0;
	pcbPtr[arrPosition].totalTimeInSystem_nano = 0;
	pcbPtr[arrPosition].timeSinceLastBurst_seconds = 0;
	pcbPtr[arrPosition].timeSinceLastBurst_nano = 0;
	pcbPtr[arrPosition].totalCPUtimeUsed_seconds = 0;
	pcbPtr[arrPosition].totalCPUtimeUsed_nano = 0;
	pcbPtr[arrPosition].isRunning = 0;
	pcbPtr[arrPosition].isTerminating = 0;
	pcbPtr[arrPosition].enqueued = 0;
	pcbPtr[arrPosition].dequeued = 0;
	pcbPtr[arrPosition].msgReceived = 0;
};

void addUserCPUtimeToClock(PCB *pcbPtr, int arrPosition, unsigned int *seconds, unsigned int *nanoseconds) {
	int billion = 1000000000;
	unsigned int tempSeconds = *seconds - pcbPtr[arrPosition].totalCPUtimeUsed_seconds;
	unsigned int tempNano = *nanoseconds - pcbPtr[arrPosition].totalCPUtimeUsed_nano;

	*seconds += tempSeconds;
	*nanoseconds += tempNano;
	
	if(*nanoseconds >= billion) {
		(*seconds)++;
		*nanoseconds = *nanoseconds - billion;
	}
};

void printForkedInfo(FILE *fPtr, PCB *pcbPtr, int arrPosition, unsigned int *seconds, unsigned int *nanoseconds) {
	if(pcbPtr[arrPosition].processPriority == 0) {
		fprintf(fPtr, "OSS: forking process %d at %u.%u with high priority\n", arrPosition, *seconds, *nanoseconds);
	}
	else {
		fprintf(fPtr, "OSS: forking process %d at %u.%u with low priority\n", arrPosition, *seconds, *nanoseconds);
	}
};

