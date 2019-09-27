#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <stdbool.h>

//driver functions
void attachToSharedMemory(int **seconds, int **nanoseconds, sem_t **sem, char **shmMsg, int shmIntId, int shmMsgId, int semId);
void getLocalTimes(int *localSecs, int *localNano, int *seconds, int *nanoseconds);	
int getRandNum(int *randNum, int *pid);
void getKillTime(int *localSecs, int *localNano, int *killSecs, int *killNano, int *randNum);
void criticalSection(sem_t *sem, int *seconds, int *nanoseconds, int *killSecs, int *killNano, int *localSecs, int *localNano, pid_t *pid, char **shmMsg, bool *jobDone);

int main(int argc, char *argv[]) {
	
	//shared mem pointers
	int *seconds = 0;
	int *nanoseconds = 0;
	char *shmMsg = NULL;
	sem_t *sem;	
	
	//list of all the variables used
	//throughout the program
	int shmIntId = 0;
	int shmMsgId = 0;
	int semId = 0;
	int randNum = 0;
	
	//set the start time
	//individual user clock
	int localSecs = 0;
	int localNano = 0;

	//to hold the time that each
	//child process will terminate
	int killSecs = 0;
	int killNano = 0;

	//flag variable to break out of
	//critical section
	bool jobDone = false;

	//seeding the random number
	//generator with each individual
	//process ID, otherwise generates
	//same number for each process
	pid_t pid = getpid();
	
	//these variables grab the values passed 
	//to the program via execlp()
	//the argv[] are locations of the variables
	//from the exec function
	shmIntId = atoi(argv[1]);
	shmMsgId = atoi(argv[2]);
	semId = atoi(argv[3]);

	//attach to shared memory, identical function to one in parent process
	attachToSharedMemory(&seconds, &nanoseconds, &sem, &shmMsg, shmIntId, shmMsgId, semId);

	//first critical section of code
	//child grabs clock values from shared memory
	//generates its termination time
	sem_wait(sem);
	getLocalTimes(&localSecs, &localNano, seconds, nanoseconds);
	sem_post(sem);

	//functions to generate the termination time
	sem_wait(sem);
	getRandNum(&randNum, &pid);
	sem_post(sem);

	getKillTime(&localSecs, &localNano, &killSecs, &killNano, &randNum);

	//second critical section
	//logic explained in the function
	criticalSection(sem, seconds, nanoseconds, &killSecs, &killNano, &localSecs, &localNano, &pid, &shmMsg, &jobDone);
	//unattach from and destroy
	//shared memory segments
	shmdt(sem);
	sem_destroy(sem);
	shmdt(seconds);
	shmdt(shmMsg);

return 0;
}

void attachToSharedMemory(int **seconds, int **nanoseconds, sem_t **sem, char **shmMsg, int shmIntId, int shmMsgId, int semId) {
	
	//attaches each pointer to a location in shared memory
	if((*seconds = (int *)shmat(shmIntId, NULL, 0)) == (void *)-1) {
		fprintf(stderr, "%s\n", strerror(errno));
	}
	*nanoseconds = *seconds + 1;

	if((*shmMsg = (char *)shmat(shmMsgId, NULL, 0)) == NULL) {
		fprintf(stderr, "%s\n", strerror(errno));
	}
	
	if((*sem = (sem_t *)shmat(semId, NULL, 0)) == (void *)-1) {
		fprintf(stderr, "%s\n", strerror(errno));
	}

	//this initializes the semaphores and points
	//it to a location in shared memory
	sem_init(*sem, 1, 1);
};

void getLocalTimes(int *localSecs, int *localNano, int *seconds, int *nanoseconds) {
	//grabs the seconds and nano seconds from the shared memory
	//clock and stores those values in local variables
	//the local values are used to help create the 
	//values for process termination via the 
	//shared memory clock
	*localSecs = seconds[0];
	*localNano = nanoseconds[0];
};

int getRandNum(int *randNum, int *pid) {
	srand((getpid()));
	*randNum = ((random() % (1000000 - 1)) - 1);
	return *randNum;
};

void getKillTime(int *localSecs, int *localNano, int *killSecs, int *killNano, int *randNum) {
	//this uses the values in local seconds, local
	//nanoseconds,  the random number generated above 
	//to set the process termination time
	*killSecs = *localSecs;
	*killNano = *localNano + *randNum;
	
	if(*killNano > 1000000000) {
		*killSecs += 1;
		*killNano = *killNano - 1000000000;
	}
	else if(*killNano == 1000000000) {
		*killSecs += 1;
		*killNano = 0;
	}
	
	if((*killSecs == 2 && *killNano >0) || (*killSecs > 2)) {
		exit(1);
	}
};

void criticalSection(sem_t *sem, int *seconds, int *nanoseconds, int *killSecs, int *killNano, int *localSecs, int *localNano, pid_t *pid, char **shmMsg, bool *jobDone) {

	//this is the main logic of the user process and constitutes the second critial section
	//the idea is:  now that the kill time is set, the child process will enter its critical section
	//and compare its kill time with that of the time in the shared memory clock
	//if the kill time is not up yet, the child process will then signal the semaphore, but
	//stay within the while loop
	//it will then call the wait signal and check the clock again
	//
	//if the kill timer is up based on the shared memory clock, then this process will try to write 
	//a shared memory message that the parent process will check
	//
	//if shmMsg is null, then it will write to it, and signal the semaphore and break out of the loop
	//otherwise, it will wait until shmMsg is NULL, then write to it, and wait for the parent process to 
	//allow it to terminate
	while(1) {
		sem_wait(sem);
		if((*seconds > *killSecs) || (*seconds >= *killSecs && *nanoseconds >= *killNano)) {
			if(*shmMsg[0] == '\0') {
				*jobDone = true;
				snprintf(*shmMsg, (sizeof(char) * 100), "%d %d %d", *pid, *killSecs, *killNano);
				if((strlen(*shmMsg) != '\0')) {
				}
			}
		}
		if(*jobDone == true) {
			sem_post(sem);
			break;
		}
		sem_post(sem);
	}
};
