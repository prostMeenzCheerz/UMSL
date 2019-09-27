#include "functions.h"

void createSharedMemKeys(key_t *sPtrKey, key_t *semKey, key_t *timeKey, key_t *qKey) {
	*sPtrKey = ftok(".", 'P');
	*semKey = ftok(".", 'S');
	*timeKey = ftok(".", 'T');
	*qKey = ftok(".", 'Q');
};

void createSharedMem(int *strctMemId, int *semMemId, int *timeMemId, int *qMemId, key_t sPtrKey, key_t semKey, key_t timeKey, key_t qKey) {
	*strctMemId = shmget(sPtrKey, STRUCT_ARRAY_SIZE, 0777 | IPC_CREAT);
	*semMemId = shmget(semKey, SEM_MEM_SIZE, 0777 | IPC_CREAT);
	*timeMemId = shmget(timeKey, CLOCK_MEM_SIZE, 0777 | IPC_CREAT);
	*qMemId = shmget(qKey, sizeof(int), 0777 | IPC_CREAT);
};

void attachToSharedMem(PCB **sPtr, sem_t **sem, unsigned int **seconds, unsigned int **nanoseconds, int **quantum, int strctMemId, int semMemId, int timeMemId, int qMemId) {
	*sPtr = (PCB *)shmat(strctMemId, NULL, 0);
	*sem = (sem_t *)shmat(semMemId, NULL, 0);
	*seconds = (unsigned int *)shmat(timeMemId, NULL, 0);
	*nanoseconds = *seconds + 1;
	*quantum = (int *)shmat(qMemId, NULL, 0);
};

void initializeSemaphore(sem_t **sem) {
	sem_init(*sem, 1, 1);
};

void unattachFromMem(sem_t *sem, PCB *sPtr, unsigned int *seconds, int *quantum) {
	sem_destroy(sem);
	shmdt(sPtr);
	shmdt(sem);
	shmdt(seconds);
	shmdt(quantum);
};
