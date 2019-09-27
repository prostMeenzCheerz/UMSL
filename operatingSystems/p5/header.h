#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <semaphore.h>
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

#ifndef SH_CLOCK_SZ 
#define SH_CLOCK_SZ (sizeof(int) * 2)
#endif

#ifndef SEM_MEM_SZ
#define SEM_MEM_SZ sizeof(int)
#endif

#ifndef MSG_BUFF_SZ
#define MSG_BUFF_SZ (sizeof(char) * 100)
#endif

typedef struct msgQ {
	long msg_type;
	char msg_txt[MSG_BUFF_SZ];
}msgQ;

typedef struct PCB {
	pid_t userPid;
	int processNum;
	int pageTable[32];
}PCB;

extern msgQ msg;
extern PCB pcb;

//global functions
void getKeys(key_t *mKey, key_t *memKey, key_t *semKey);
void pointToShMem(unsigned int **seconds, unsigned int **nanoseconds, sem_t **sem, int memID, int semID);
void unattachFromMem(unsigned int *seconds, sem_t *sem);

//oss functions
void getOpt(int argc, char *argv[], int *option, int *numProcesses);
void openSharedMem(int *msgID, int *memID, int *semID, key_t msgKey, key_t memKey, key_t semKey);
void initializeSemaphore(sem_t **sem);
void initializeFilePtr(FILE **fPtr);
void destroySharedMem(int *memID, int *msgID, int *semID);
void createExecArgs(char msgMem[], char memMem[], char semMem[], int msgID, int memID, int semID);
void incrementClockNoPageFault(unsigned int *seconds, unsigned int *nanoseconds);
void incrementClockPageFault(unsigned int *seconds, unsigned int *nanoseconds);
void incrementClock(unsigned int *seconds, unsigned int *nanoseconds);
void getForkTime(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *forkSeconds, unsigned int *forkNano);
pid_t forkChild(pid_t pid, pid_t pidArr[], char msgMem[], char memMem[], char semMem[]);
void getArrPosition(pid_t pidArr[], int *pos, int numProcesses);
void pidInVector(pid_t pidArr[], int pos, pid_t childPid);
void intializePidArr(pid_t pidArr[], int numProcesses);
PCB** createDataForPtrs(PCB *pcbArr[], int numProcesses);
PCB* initializePcbArr(PCB *(*pcbPtr)[], int numProcesses);
void getInfo(char string[], pid_t *tempPid, int *request, int *memLocation, int *page, int *times, int *deaths);
void handleRequest(FILE **fPtr, pid_t pidArr[], int frameTable[][3], pid_t tempPid, PCB *(*pcbPtr)[], int i, int *j, int memLocation, int request, int *fTablePtr, 
	unsigned int *seconds, unsigned int *nanoseconds, int *access, int *children, int page, int *pageFaults, int *lines);
void removeFromFTable(int frameTable[][3], pid_t tempPid);
void clearStructPtr(PCB *(*pcbPtr)[], int *i, pid_t tempPid, int numProcesses);
void printTable(FILE **fPtr, int *access, int frameTable[][3]);

//process functions
void getRequestTime(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *requestSeconds, unsigned int *requestNano);
void makeRequest(int *readWrite, int *requestMem);
void buildSendRequest(msgQ *msg, long int sendMsgType, int readWrite, int requestMem, char requestType[], char memRequest[]);
bool termCheck();
void sendTermRequest(msgQ *msg, long int sendMsgType);
#endif
