#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>

#define BUFF_SZ (sizeof(int) *2)

int *sharedInt1;
int *sharedInt2;
int shmid;

static void alarmHandler(int signo) {
	printf("seconds: %d milliseconds: %d\n", *sharedInt1, *sharedInt2);
	shmdt(sharedInt1);
	shmctl(shmid, IPC_RMID, NULL);
	kill(0, SIGTERM);
};

static void commandLineKill(int signo) {
	printf("seconds: %d milliseconds: %d\n", *sharedInt1, *sharedInt2);
	shmdt(sharedInt1);
	shmctl(shmid, IPC_RMID, NULL);
	kill(0, SIGTERM);
};

int main(int argc, char *argv[]) {

	signal(SIGALRM, alarmHandler);
	signal(SIGINT, commandLineKill);
	alarm(2);

	int num1 = 0;
	int num2 = 0;
	
	sharedInt1 = &num1;
	sharedInt2 = &num2;

	char arg1[10];

	key_t key;
	key = ftok(".", 'A');
	printf("key in master %d\n", key);
	
	pid_t pid = 0; 
	pid_t wpid;
	
	int i, totalNumChildren, numChildrenAtATime, option, status;
	int counter = 1;
	int totalCounter = 0;
	while ((option = getopt(argc, argv, "hn:s:")) != -1)
		switch (option) {
		case 'h':
			printf("To run the program, first type 'make' on the command line.\n");
			printf("Then, type ./master -n x -s x.  The -n x will handle the total number of forked processes, and\n");
			printf("the -s x will handle the total number of processes running at any given time.\n");
			printf("You may also run the program by ommitting the -s x option.\n");
			exit(0);
		case 'n':
			totalNumChildren = atoi(argv[2]);
			break;
		case 's':
			numChildrenAtATime = atoi(argv[4]);
			break;
		case '?':
			break;
		default:
			break;
	}

	
	//shmget returns an identifier in shmid
	shmid = shmget(key, BUFF_SZ, 0777 | IPC_CREAT);
	printf("shmid from master %d\n", shmid);
	if(shmid == -1) {
		perror("shmget\n");
		exit(1);
	}

	//creating the shared integers for the clock
	sharedInt1 = (int *) shmat(shmid, NULL, 0);
	printf("value at sharedInt1 %p\n", sharedInt1);
	sharedInt2 = sharedInt1 + 1;
	
	if(totalNumChildren > 20) {
		totalNumChildren = 20;
	}

	snprintf(arg1, 10, "%d", totalNumChildren);
	
	if(totalNumChildren <= numChildrenAtATime) {
		for(i = 0; i < totalNumChildren; i++) {
			if((pid = fork()) == -1){
				perror("Failed to fork...Exiting\n");
				exit(1);
			}
			else if(pid == 0) {
				execlp("./child", "./child", arg1, (char *)NULL);
			}
		}
	}
	else {
		do {
			if(counter == numChildrenAtATime) {
				wait(&status);
				counter--;
			}

			if((pid = fork()) == -1){
				perror("Failed to fork...Exiting\n");
				exit(1);
			}
			else if(pid == 0) {
				execlp("./child", "./child", arg1, (char *)NULL);
			}
			counter++;
			totalCounter++;
		}while(totalCounter < totalNumChildren);
	}

	while((wpid = wait(&status)) > 0);

	printf("total seconds:%d	total milliseconds:%d\n", *sharedInt1, *sharedInt2);

	//detach from shared memory
	shmdt(sharedInt1);

	//destroy the shared memory
	shmctl(shmid, IPC_RMID, NULL);

	return 0;
}
