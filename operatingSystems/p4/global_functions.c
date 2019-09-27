#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>

#include "header.h"
	*sharedTimeKey = ftok(".", 12);
	*msgQkey = ftok(".", 34);
	*termKey = ftok(".", 07);
	*userPcbKey = ftok(".", 56);
};

void createSharedMem(int *clockMemLocation, int *msgID, int *termID, int *shPcbMem, key_t sharedClockKey, key_t msgQkey, key_t termKey, key_t userPcbKey) {
	*clockMemLocation = shmget(sharedClockKey, CLOCK_MEM_BUFF, 0777 | IPC_CREAT);
	*msgID = msgget(msgQkey, 0777 | IPC_CREAT);
	*termID = msgget(termKey, 0777 | IPC_CREAT);
	*shPcbMem = shmget(userPcbKey, USER_PCB_ARR, 0777 | IPC_CREAT);
};

void pointToSharedMem(unsigned int **seconds, unsigned int **milliseconds, userPCB **userPcbArrPtr, int clockMemLocation, int shPcbMem) {
	*seconds = (unsigned int *)shmat(clockMemLocation, NULL, 0);
	*milliseconds = *seconds + 1;
	*userPcbArrPtr = (userPCB *)shmat(shPcbMem, NULL, 0);
};

void unattachFromMem(unsigned int *seconds, userPCB *userPcbArrPtr) {
	shmdt(seconds);
	shmdt(userPcbArrPtr);
};

void destroySharedMem(int *clockMemLocation, int *msgID, int *termID, int *shPcbMem) {
	shmctl(*clockMemLocation, IPC_RMID, NULL);
	msgctl(*msgID, IPC_RMID, NULL);
	msgctl(*termID, IPC_RMID, NULL);
	shmctl(*shPcbMem, IPC_RMID, NULL);
};
