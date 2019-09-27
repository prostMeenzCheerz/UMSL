#include "functions.h"

void getUserTotalSystemTime(PCB *pcbPtr, int pcbPosition, unsigned int *seconds, unsigned int *nanoseconds, unsigned int *tempTimeInSystem_seconds, unsigned int *tempTimeInSystem_nano, sem_t *sem) {
	if((pcbPtr[pcbPosition].totalTimeInSystem_seconds == 0 && pcbPtr[pcbPosition].totalTimeInSystem_nano == 0)) {
//		printf("user: first grabbing system time %u.%u\n", *seconds, *nanoseconds);

		*tempTimeInSystem_seconds = seconds[0];
		*tempTimeInSystem_nano = nanoseconds[0];
	}
	else {
		pcbPtr[pcbPosition].totalTimeInSystem_seconds = seconds[0] - *tempTimeInSystem_seconds;
		if(*nanoseconds < *tempTimeInSystem_nano) {
			pcbPtr[pcbPosition].totalTimeInSystem_nano = *tempTimeInSystem_nano - nanoseconds[0];
		}
		else{
			pcbPtr[pcbPosition].totalTimeInSystem_nano = nanoseconds[0] - *tempTimeInSystem_nano;
		}
//		printf("user: time of life is: %u.%u\n", pcbPtr[pcbPosition].totalTimeInSystem_seconds, pcbPtr[pcbPosition].totalTimeInSystem_nano);
	}
};

void getTimeSinceLastBurst(PCB *pcbPtr, int pcbPosition, unsigned int *seconds, unsigned int *nanoseconds, unsigned int *tempTimeSinceLastBurst_seconds, unsigned int *tempTimeSinceLastBurst_nano, sem_t *sem) {
	if((pcbPtr[pcbPosition].timeSinceLastBurst_seconds == 0 && pcbPtr[pcbPosition].timeSinceLastBurst_nano == 0)) {
		*tempTimeSinceLastBurst_seconds = seconds[0];
		*tempTimeSinceLastBurst_nano = nanoseconds[0];
	}
	else {
		pcbPtr[pcbPosition].timeSinceLastBurst_seconds = seconds[0] - *tempTimeSinceLastBurst_seconds;
		if(*nanoseconds < *tempTimeSinceLastBurst_nano) {
			pcbPtr[pcbPosition].timeSinceLastBurst_nano = *tempTimeSinceLastBurst_nano - nanoseconds[0];
		}
		else{
			pcbPtr[pcbPosition].timeSinceLastBurst_nano = nanoseconds[0] - *tempTimeSinceLastBurst_nano;
		}
	}
};

void initUser(PCB *pcbPtr, int pcbPosition, unsigned int **seconds, unsigned int **nanoseconds, unsigned int *tempTimeInSystem_seconds, unsigned int *tempTimeInSystem_nano, sem_t *sem) {
	getUserTotalSystemTime(pcbPtr, pcbPosition, *seconds, *nanoseconds, tempTimeInSystem_seconds, tempTimeInSystem_nano, sem);

	pcbPtr[pcbPosition].position = pcbPosition;
	pcbPtr[pcbPosition].pid = getpid();
	pcbPtr[pcbPosition].isSet = 1;
	
};

void getTimeToWait(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *waitToQueue_seconds, unsigned int *waitToQueue_nano, sem_t *sem) {
	unsigned int tempSeconds = (rand() % 6);
	unsigned int tempNano = (rand() % 1001);
	*waitToQueue_seconds = seconds[0] + tempSeconds;
	*waitToQueue_nano = nanoseconds[0] + tempNano;
};

void getTotalCPUtimeUsed(PCB *pcbPtr, int pcbPosition, int localQuantum, sem_t *sem) {
	int billion = 1000000000;
	pcbPtr[pcbPosition].totalCPUtimeUsed_nano += localQuantum;
	if(pcbPtr[pcbPosition].totalCPUtimeUsed_nano > billion) {
		pcbPtr[pcbPosition].totalCPUtimeUsed_seconds += 1;
		pcbPtr[pcbPosition].totalCPUtimeUsed_nano = pcbPtr[pcbPosition].totalCPUtimeUsed_nano - billion;
	}
};

void setQuantum(PCB *pcbPtr, int pcbPosition, int *quantum, int *localQuantum, sem_t *sem) {
	if(pcbPtr[pcbPosition].processPriority == 0) {
		*localQuantum = (quantum[0] / 2);
	}
	else {
		*localQuantum = quantum[0];
	}
//	printf("user: %d with priority %d has local quantum %d\n", getpid(), pcbPtr[pcbPosition].processPriority, *localQuantum);

};

void getAction(PCB **pcbPtr, int pcbPosition, unsigned int **seconds, unsigned int **nanoseconds, int localQuantum, unsigned int *waitToQueue_seconds, unsigned int *waitToQueue_nano,
	unsigned int *tempTimeInSystem_seconds, unsigned int *tempTimeInSystem_nano, unsigned int *tempTimeSinceLastBurst_seconds, unsigned int *tempTimeSinceLastBurst_nano, int *totalQuantumUsed, sem_t *sem) {

	(*pcbPtr[pcbPosition]).flag = 0;

	int randNum = (rand() % 100);
	int billion = 1000000000;
	if(randNum <= 9) {                        //0
		(*pcbPtr[pcbPosition]).isTerminating = 1;
		(*pcbPtr[pcbPosition]).isRunning = -1;
		getTimeSinceLastBurst(*pcbPtr, pcbPosition, *seconds, *nanoseconds, tempTimeSinceLastBurst_seconds, tempTimeSinceLastBurst_nano, sem);
	}
	else if(randNum > 9 && randNum <= 39) {   //1
		getTimeSinceLastBurst(*pcbPtr, pcbPosition, *seconds, *nanoseconds, tempTimeSinceLastBurst_seconds, tempTimeSinceLastBurst_nano, sem);
		(*pcbPtr[pcbPosition]).isTerminating = 1;
		(*pcbPtr[pcbPosition]).isRunning = -1;
		*totalQuantumUsed += localQuantum;
		(*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano += localQuantum;
		if((*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano >= billion) {
			(*pcbPtr[pcbPosition]).totalCPUtimeUsed_seconds++;
			(*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano = (*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano - billion;
		}
		getTotalCPUtimeUsed(*pcbPtr, pcbPosition, localQuantum, sem);
	}	
	else if(randNum > 39 && randNum <= 69) {  //2
		getTimeSinceLastBurst(*pcbPtr, pcbPosition, *seconds, *nanoseconds, tempTimeSinceLastBurst_seconds, tempTimeSinceLastBurst_nano, sem);
		getTimeToWait(*seconds, *nanoseconds, waitToQueue_seconds, waitToQueue_nano, sem);
		(*pcbPtr[pcbPosition]).isSet = 0;
		(*pcbPtr[pcbPosition]).isRunning = -1;

	}
	else if(randNum > 69) { 		  //3	
		getTimeSinceLastBurst(*pcbPtr, pcbPosition, *seconds, *nanoseconds, tempTimeSinceLastBurst_seconds, tempTimeSinceLastBurst_nano, sem);
		randNum = ((rand() % 98) + 1);
		float percentTimeUsed = randNum;
		percentTimeUsed = (int)percentTimeUsed;
		percentTimeUsed = ((percentTimeUsed / 100) * localQuantum);
		*totalQuantumUsed += (int)percentTimeUsed;
		(*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano += (int)percentTimeUsed;
		if((*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano >= billion) {
			(*pcbPtr[pcbPosition]).totalCPUtimeUsed_seconds++;
			(*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano = (*pcbPtr[pcbPosition]).totalCPUtimeUsed_nano - billion;
		}
		(*pcbPtr[pcbPosition]).isRunning = -1;
		getTotalCPUtimeUsed(*pcbPtr, pcbPosition, localQuantum, sem);
	}
};

