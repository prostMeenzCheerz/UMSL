#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <semaphore.h>
#include <stdbool.h>
#include <signal.h>

typedef struct PCB {

	pid_t pid;

	int position;
	int processPriority;
	
	int isSet;
	int inQueue;
	int flag;

	unsigned int totalTimeInSystem_seconds;
	unsigned int totalTimeInSystem_nano;

	unsigned int timeSinceLastBurst_seconds;
	unsigned int timeSinceLastBurst_nano;

	unsigned int totalCPUtimeUsed_seconds;
	unsigned int totalCPUtimeUsed_nano;

	int isRunning;
	int isTerminating;

	int enqueued;
	int dequeued;

	int msgReceived;

}PCB;


extern PCB pcbArr[18];
extern PCB *sPtr;
extern sem_t *sem;

//struct for queue
typedef struct QNodeType {
	PCB *sPtr;
	struct QNodeType *next;
}QNode;


#define STRUCT_ARRAY_SIZE (sizeof(PCB) *18)
#define SEM_MEM_SIZE sizeof(int)
#define CLOCK_MEM_SIZE (sizeof(unsigned int) * 2)
#define QUEUE_SIZE 18

//FOR THE QUEUE
//definition for FALSE
#ifndef FALSE
#define FALSE (0)
#endif
//definition for TRUE
#ifndef TRUE
#define TRUE (!FALSE)
#endif

extern pid_t pid;
extern key_t sPtrKey, semKey, timeKey, qKey;
extern int strctMemId, semMemId, timeMemId, qMemId, forkedP;
extern unsigned int *seconds, *nanoseconds;
extern int *quantum, forkTimeSecs, forkTimeNano, arrPosition, terminatingPID, bitVector[18];
extern char sharedStrctMem[10], sharedSemMem[10], sharedClockMem[10], childArrayPosition[10], timeQuantum[10];
extern bool isFull, processRunning, processTerminating, isStopped;
extern FILE *fPtr;

//these functions are shared by both executables: oss and user, the definitions can be found in global_functions.c
void createSharedMemKeys(key_t *sPtrKey, key_t *semKey, key_t *timeKey, key_t *qKey);
void createSharedMem(int *strctMemId, int *semMemId, int *timeMemId, int *qMemId, key_t sPtrKey, key_t semKey, key_t timeKey, key_t qKey);
void attachToSharedMem(PCB **sPtr, sem_t **sem, unsigned int **seconds, unsigned int **nanoseconds, int **quantum, int strctMemId, int semMemId, int timeMemId, int qMemId);
void initializeSemaphore(sem_t **sem);
void unattachFromMem(sem_t *sem, PCB *sPtr, unsigned int *seconds, int *quantum);

//these functions are for oss.c, the definitions can be found in oss_functions.c
void createExecArgs(char *sharedStrctMem, char *sharedSemMem, char *sharedClockMem, char *childArrayPosition, char *timeQuantum,  int strctMemId, int semMemId, int timeMemId, int qMemId, int arrPosition);
void getArrayPosition(PCB *pcbPtr, int *arrPosition);
void updateVector(int bitVector[]);
bool isVectorFull(int bitVector[], bool *isFull);
bool isVectorEmpty(int bitVector[], bool *hasValue);
void setThePriority(PCB *pcbPtr, int arrPosition);
void forkChild(char *sharedStrctMem, char *sharedSemMem, char *sharedClockMem, char *childArrayPosition, char *timeQuantum, PCB **pcbPtr, int *arrPosition, int strctMemId, int semMemId, 
	int timeMemId, int qMemId, int bitVector[], pid_t pid);

void getRandomForkTime(unsigned int *seconds, unsigned int *nanoseconds, int *forkTimeSecs, int *forkTimeNano);
void incrementTheClock(unsigned int **seconds, unsigned int **nanoseconds);
bool checkRunningProcess(PCB *pcbPtr, bool *processRunning);
void enqueueReadyProcess(PCB *pcbPtr, FILE *fPtr);
bool checkForStoppedProcess(PCB *pcbPtr, int *arrPosition, int *dequeued, bool *isStopped);
void checkForTerminatingProcess(PCB *pcbPtr, int *arrPosition, int *terminatingPID, bool *processTerminating);
void decrementBitVector(int bitVector[]);
void clearStruct(PCB *pcbPtr, int arrPosition);
void addUserCPUtimeToClock(PCB *pcbPtr, int arrPosition, unsigned int *seconds, unsigned int *nanoseconds);
void printForkedInfo(FILE *fPtr, PCB *pcbPtr, int arrPosition, unsigned int *seconds, unsigned int *nanoseconds);
void removeMemSegments(int *strctMemId, int *semMemId, int *timeMemId, int *qMemId);

//queue functions used by oss
void InitLPQueue();
void InitHPQueue();

void ClearLPQueue();
void ClearHPQueue();

int EnqueueLP(PCB *sPtr, int arrPosition, FILE *fPtr);
int EnqueueHP(PCB *sPtr, int arrPosition, FILE *fPtr);

pid_t DequeueLP(PCB *sPtr, int *arrPosition, FILE *fPtr);
pid_t DequeueHP(PCB *sPtr, int *arrPosition, FILE *fPtr);

int isLPQueueEmpty();
int isHPQueueEmpty();

int isLPQueueFull();
int isHPQueueFull();

//these functions are for user.c, the defintions can be found in user_functions.c
void initUser(PCB *pcbPtr, int pcbPosition, unsigned int **seconds, unsigned int **nanoseconds, unsigned int *tempTimeInSystem_seconds, unsigned int *tempTimeInSystem_nano, sem_t *sem);
void setQuantum(PCB *pcbPtr, int pcbPosition, int *quantum, int *localQuantum, sem_t *sem);
void getUserTotalSystemTime(PCB *pcbPtr, int pcbPosition, unsigned int *seconds, unsigned int *nanoseconds, unsigned int *tempTimeInSystem_seconds, unsigned int *tempTimeInSystem_nano, sem_t *sem);
void getTimeSinceLastBurst(PCB *pcbPtr, int pcbPosition, unsigned int *seconds, unsigned int *nanoseconds, unsigned int *tempTimeSinceLastBurst_seconds, unsigned int *tempTimeSinceLastBurst_nano, sem_t *sem);
void getTotalCPUtimeUsed(PCB *pcbPtr, int pcbPosition, int localQuantum, sem_t *sem);
void getAction(PCB **pcbPtr, int pcbPosition, unsigned int **seconds, unsigned int **nanoseconds, int localQuantum, unsigned int *waitToQueue_seconds, unsigned int *waitToQueue_nano,
	unsigned int *tempTimeInSystem_seconds, unsigned int *tempTimeInSystem_nano, unsigned int *tempTimeSinceLastBurst_seconds, unsigned int *tempTimeSinceLastBurst_nano, int *totalQuantumUsed, sem_t *sem);
void getTimeToWait(unsigned int *seconds, unsigned int *nanoseconds, unsigned int *waitToQueue_seconds, unsigned int *waitToQueue_nano, sem_t *sem);
#endif
