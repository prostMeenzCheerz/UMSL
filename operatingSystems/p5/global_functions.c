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

#include "header.h"

void pointToShMem(unsigned int **seconds, unsigned int **nanoseconds, sem_t **sem, int memID, int semID) {
	*seconds = (unsigned int *)shmat(memID, NULL, 0);
	*nanoseconds = *seconds + 1;
	*sem = (sem_t *)shmat(semID, NULL, 0);
};

void unattachFromMem(unsigned int *seconds, sem_t *sem) {
	shmdt(seconds);
	sem_destroy(sem);
};

