#ifndef HEADER_H
#define HEADER_H

#include <stdbool.h>

#ifndef CLOCK_MEM_BUFF
#define CLOCK_MEM_BUFF (sizeof(unsigned int) * 2)
#endif

#ifndef MSG_BUFF_SZ
#define MSG_BUFF_SZ 100
#endif

#ifndef USER_PCB_ARR
#define USER_PCB_ARR (sizeof(userPCB) * 18)
#endif

//FOR THE QUEUE
//definition for FALSE
#ifndef FALSE
#define FALSE (0)
#endif
//definition for TRUE
#ifndef TRUE
#define TRUE (!FALSE)
#endif

//oss structure to keep track of control resource values
typedef struct ossPCB {
	int mainResources[20];
	int resourcesRemaining[20];
}ossPCB;

//user structure to hold their resource values
typedef struct userPCB {
	pid_t userPid;
	int arrPosition;
	int resourceRequested;
	int actionTaken;
	int isTerminating;
	int isBlocked;
	int userResources[20];
	int uResourcesMaxBounds[20];
	unsigned int timeEnqueuedSeconds;
	unsigned int timeEnqueuedMilli;
	unsigned int timeDequeuedSeconds;
	unsigned int timeDequeuedMilli;
}userPCB;

//message queue struct oss will use to send messages
typedef struct msg_buffer {
	long msg_type;
	char msg_txt[MSG_BUFF_SZ];
}msg;

//message queue specifically for terminating process
typedef struct term_msg {
	long term_type;
	char term_txt[MSG_BUFF_SZ];
}term;

//struct node for blocked queue
typedef struct QNodeType {
	userPCB *sPtr;
	struct QNodeType *next;
}QNode;

//extern variable so oss can have its own ossPCB instance
extern ossPCB oss;

//userArr[] and *userPcbArrPtr will be in shared memory and used  to keep track of each user's resources via an array and array pointer
//by both the oss and user processes
extern userPCB *userPcbArrPtr;

//extern message queue to for each process to have their own instance
extern msg message;
extern term termMessage;

//globally used functions
void createSharedMemKeys(key_t *sharedClockKey, key_t *msgQkey, key_t *termKey, key_t *userPcbKey);
void createSharedMem(int *clockMemLocation, int *msgID, int *termID, int *shPcbMem, key_t sharedClockKey, key_t msgQkey, key_t termKey, key_t userPcbKey);
void pointToSharedMem(unsigned int **seconds, unsigned int **milliseconds, userPCB **userPcbArrPtr, int clockMemLocation, int shPcbMem);
void unattachFromMem(unsigned int *seconds, userPCB *userPcbArrPtr);
void destroySharedMem(int *clockMemLocation, int *msgID, int *termID, int *shPcbMem);

//oss functions
void getOpt(int argc, char *argv[], int *option, int *verbose);
void initializeFilePtr(FILE **fPtr);
void makeResources(ossPCB *ossPtr, int resourceArrSize);
void createExecArgs(char clockMem[], char msgMem[], char termMem[], char uPcbMem[], int clockMemLocation, int msgID, int termID, int shPcbMem);
void getForkTime(unsigned int *seconds, unsigned int *milliseconds, unsigned int *forkSeconds, unsigned int *forkMilli);
void getOssMaxBound(ossPCB *ossPtr, userPCB *userPtr, int arrPosition, int bound, int resourceArrSize, char maxBound[]);
void incrementVector(int bitVector[], int arrPosition);
void decrementVector(int bitVector[], int arrPosition);
void getArrPosition(int bitVector[], int vectorSize, int *arrPosition);
void forkChild(pid_t pid, char clockMem[], char msgMem[], char termMem[], char uPcbMem[], int arrPosition, char maxBound[], ossPCB *ossPtr, int resourceArrSize, int bound, int bitVector[]);
void incrementClock(unsigned int *seconds, unsigned int *milliseconds);
void resetTerminatingPcb(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition);
void releaseAllResources(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition);
void getResource(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition, int resource);
void releaseResource(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition, int resource, int verbose, FILE **fPtr, int *lines, unsigned int *seconds, unsigned int *milliseconds);
bool bankersAlgorithm(userPCB *userPcbArrPtr, ossPCB *ossPtr, int arrPosition);
void outputTable(FILE **fPtr, userPCB *userPcbArrPtr);

//user functions
void getUserMaxBound(userPCB *userPcbArrPtr, int pos, int arrSize, char bound[]);
void init_user(userPCB *userPcbArrPtr, int pos);
int action(userPCB *userPcbArrPtr, int pos);
void getRequestedResource(userPCB *userPcbArrPtr, int pos, int *randomResource);
void getReleasedResource(userPCB *userPcbArrPtr, int pos, int *randomResource);
void pickATime(unsigned int *seconds, unsigned int *milliseconds, unsigned int *actionSeconds, unsigned int *actionMilli);

//queue functions
void InitQueue();
void Enqueue(userPCB *userPcbArrPtr, int arrPosition, unsigned int *seconds, unsigned int *milliseconds);
void Dequeue(userPCB *userPcbArrPtr, unsigned int *seconds, unsigned int *milliseconds, FILE **fPtr, int *lines);
void searchQueue(userPCB *userPcbArrPtr, ossPCB *ossPtr, int *arrPosition, int resource, unsigned int *seconds, unsigned int *milliseconds, FILE **fPtr, int *lines, int *run);
int isQueueEmpty();
#endif
