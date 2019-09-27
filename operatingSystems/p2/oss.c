#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>

//constants for the of the buffers used for 
//each shared memory segment
#define INTBUFF_SZ (sizeof(int) * 2)
#define MSGBUFF_SZ (sizeof(char) * 100)
#define SEMBUFF_SZ (sizeof(int))

volatile sig_atomic_t signalCalled = 0;

//signal handler functions
static void sigHandler(int signo);
void cleanup(int *seconds, char *shmMsg, sem_t *sem, int *shmIntId, int *shmMsgId, int *semId);

//driver functions
void getOpt(int argc, char *argv[], FILE **output, int *option, int *x, int *z, char **filename);
void createKeys(key_t *key1, key_t *key2, key_t *key3);
void createSharedMemory(int *shmIntId, int *shmMsgId, int *semId, key_t key1, key_t key2, key_t key3);
void attachToSharedMemory(int **seconds, int **nanoseconds, sem_t **sem, char **shmMsg, int shmIntId, int shmMsgId, int semId);
void createExecArgs(char *iMemKey, char *sMemKey, char *semKey, int shmIntId, int shmMsgId, int semId);
void createInitialNumProcesses(int x, pid_t pid, char *iMemKey, char *sMemKey, char *semKey);
void ossCriticalSection(int **nanoseconds, int **seconds, sem_t *sem, char **shmMsg, char *childPid, char *childSeconds, char *childNano, FILE **output, int *x, int *totalNumProcesses, pid_t *pid,
	char *iMemKey, char *sMemKey, char *semKey, int *currentNumProcesses, int *status, int *shmIntId, int *shmMsgId, int *semId);

int main(int argc, char *argv[]) {
	
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = sigHandler;

	//pointers for the shared memory
	int *seconds = 0;
	int *nanoseconds = 0;
	char *shmMsg = NULL;
	sem_t *sem;

	//variables to hold shared mem location IDs
	int shmIntId = 0;
	int shmMsgId = 0;
	int semId = 0;

	//initializing variables used
	//throughout program
	int totalNumProcesses = 0;
	int currentNumProcesses = 0;
	int x = 0;
	int z = 0;
	int option = 0;
	int status;

	//file pointer and filename buffer
	FILE *output;
	char *filename = NULL;
	
	//variables to hold the shared mem keys
	key_t key1, key2, key3;

	//variables to hold the
	//various process IDs
	pid_t pid = 0;
	pid_t wpid = 0;
	
	//char buffers to store the values getting
	//passed to execlp()
	char iMemKey[11];
	char sMemKey[11];
	char semKey[11];

	//char buffers to tokenize shared mem
	//string from user
	char childPid[10];
	char childSeconds[10];
	char childNano[10];

	//functions to (first) grab command line arguments
	//(second) to create the shared the memory segments
	//(third) to initialize the pointers to point to
	//the shared memory segments
	getOpt(argc, argv, &output, &option, &x, &z, &filename);
	createKeys(&key1, &key2, &key3);
	createSharedMemory(&shmIntId, &shmMsgId, &semId, key1, key2, key3);
	attachToSharedMemory(&seconds, &nanoseconds, &sem, &shmMsg, shmIntId, shmMsgId, semId);
	createExecArgs(iMemKey, sMemKey, semKey, shmIntId, shmMsgId, semId);
	
	//signal functions
	sigaction(SIGINT, &act, 0);
	sigaction(SIGALRM, &act, 0);
	alarm(z);

	//first called to create the initial
	//round of child processes
	createInitialNumProcesses(x, pid, iMemKey, sMemKey, semKey);

	//setting these variables to keep track 
	//of how many total processes have been 
	//made and how many are running at one time
	totalNumProcesses = x;
	currentNumProcesses = x;
	ossCriticalSection(&nanoseconds, &seconds, sem, &shmMsg, childPid, childSeconds, childNano, &output, &x, &totalNumProcesses, &pid, iMemKey, sMemKey, semKey, &currentNumProcesses, &status, 
		&shmIntId, &shmMsgId, &semId);

	//making sure all of the child procsses
	//terminate before clearing the shared memory
	while((wpid = wait(&status)) > 0);

	//detach the pointers from their
	//shared memory segments
	shmdt(seconds);
	shmdt(shmMsg);
	shmdt(sem);
	sem_destroy(sem);

	//destroy the shared memory segments
	shmctl(shmIntId, IPC_RMID, NULL);
	shmctl(shmMsgId, IPC_RMID, NULL);
	shmctl(semId, IPC_RMID, NULL);

	//close the file stream
	fclose(output);
return 0;
}

static void sigHandler(int signo) {
	signalCalled = 1;
};

void cleanup(int *seconds, char *shmMsg, sem_t *sem, int *shmIntId, int *shmMsgId, int *semId) {

	shmdt(seconds);
	shmdt(shmMsg);
	shmdt(sem);
	sem_destroy(sem);

	shmctl(*shmIntId, IPC_RMID, NULL);
	shmctl(*shmMsgId, IPC_RMID, NULL);
	shmctl(*semId, IPC_RMID, NULL);
	
	printf("terminating\n");
	
	kill(0, SIGTERM);
};

void getOpt(int argc, char *argv[], FILE **output, int *option, int *x, int *z, char **filename) {
	
	//initialize the getopt vars
	//this also sets default values
	//if certain command line options
	//aren't used
	*option = 0;
	*x = 5;
	*z = 2;
	*filename = (char *)malloc(sizeof(char) * 50);
	*filename = "log.txt";
	
	//switch statement to be used with the getopt function
	while ((*option = getopt(argc, argv, "hs:l:t:f:")) != -1)
		switch(*option) {
		case 'h':
			printf("To run the program, type 'make' on the command line, then './oss'\n");
			printf("To add options to the program type: -s (number), -l (filename), or -t (number)\n");
			printf("for number of process run at one, to choose log file name, or change program\n");
			printf("timer respectively.\n");
			exit(1);
		case 's':
			*x = atoi(optarg);
			break;
		case 'l':
			*filename = optarg;
			strcpy(*filename, ".txt");
			break;
		case 't':
			*z = atoi(optarg);
			break;
		case '?':
			break;
		default:
			break;
		}

	//open the file stream
	*output = fopen(*filename, "a");
};

void createKeys(key_t *key1, key_t *key2, key_t *key3) {
	
	//generate the keys for opening
	//the shared memory segments
	//ftok uses the current directory
	//and specified char to 
	//generate the key
	*key1 = ftok(".", 'I');
	*key2 = ftok(".", 'S');
	*key3 = ftok(".", '4');
};

void createSharedMemory(int *shmIntId, int *shmMsgId, int *semId, key_t key1, key_t key2, key_t key3) {

	//creates the shared memory segments
	//using each specific key, with open permissions
	*shmIntId = shmget(key1, INTBUFF_SZ, 0777 | IPC_CREAT);
	if(*shmIntId == -1) {
		fprintf(stderr, "error getting shmIntId %s\n", strerror(errno));
	}
	
	*shmMsgId = shmget(key2, MSGBUFF_SZ, 0777 | IPC_CREAT);
	if(*shmMsgId == -1) {
		fprintf(stderr, "error getting shmMsgId %s\n", strerror(errno));
	}
	
	*semId = shmget(key3, SEMBUFF_SZ, 0777 | IPC_CREAT);
	if(*semId == -1) {
		fprintf(stderr, "error getting semId %s\n", strerror(errno));
	}
};

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
	//the value of '1' in arg 2 means the semaphore will be shared between processes
	//the value of '1' in arg3 means the semaphore will hold the value 1
	sem_init(*sem, 1, 1);
};

void createExecArgs(char *iMemKey, char *sMemKey, char *semKey, int shmIntId, int shmMsgId, int semId) {
	
	//using the char buffers to hold the shmIDs
	//and passing the strings through execlp()
	snprintf(iMemKey, 11, "%d", shmIntId);
	snprintf(sMemKey, 11, "%d", shmMsgId);
	snprintf(semKey, 11, "%d", semId);
};

void createInitialNumProcesses(int x, pid_t pid, char *iMemKey, char *sMemKey, char *semKey) {
	
	int i;
	
	//loop to fork off the child
	//processes, pid == 0 is a successful
	//fork, then the child will exec into
	//a unique process
	for(i = 0; i < x; i++) {
		if((pid = fork()) == -1) {
			perror("Failed to fork\n");
			exit(1);
		}
		else if(pid == 0) {
			execlp("./user", "./user", iMemKey, sMemKey, semKey, (char *)NULL);
		}
	}
};

void createMoreProcesses(int x, pid_t pid, int currentNumProcesses, char *iMemKey, char *sMemKey, char *semKey) {
	
	int i = 0;
	//loop to fork off the child
	//processes, pid == 0 is a successful
	//fork, then the child will exec into
	//a unique process
	for(i = currentNumProcesses; i < x; i++) {
		if((pid = fork()) == -1) {
			perror("Failed to fork\n");
			exit(1);
		}
		else if(pid == 0) {
			execlp("./user", "./user", iMemKey, sMemKey, semKey, (char *)NULL);
		}
	}
};

void ossCriticalSection(int **nanoseconds, int **seconds, sem_t *sem, char **shmMsg, char *childPid, char *childSeconds, char *childNano, FILE **output, int *x, int *totalNumProcesses, pid_t *pid,
	char *iMemKey, char *sMemKey, char *semKey, int *currentNumProcesses, int *status, int *shmIntId, int *shmMsgId, int *semId) {
		
	do {	
		//check if signal has been called
		if(signalCalled == 1) {
			cleanup(*seconds, *shmMsg, sem, shmIntId, shmMsgId, semId);
		}

		//enter critical section
		//and increment the clock
		sem_wait(sem);
		*currentNumProcesses = *x;
		**nanoseconds += 500;
		if(**nanoseconds >= 1000000000) {
			**seconds += 1;
			**nanoseconds = **nanoseconds - 1000000000;
		}
		else if(**nanoseconds == 1000000000) {
			**seconds += 1;
			**nanoseconds = 0;
		}
		//check shared memory message to see if child
		//process sent message that it is ready
		//to be terminated
		if((*shmMsg[0] != '\0')) {
			//if shared memory message is not empty:
			//tokenize the information sent from child
			strcpy(childPid, strtok(*shmMsg, " "));
			strcpy(childSeconds, strtok(NULL, " "));
			strcpy(childNano, strtok(NULL, " "));
			pid_t childProcess= atoi(childPid);
			//use the tokenized info to build a string to write to our log file
			fprintf(*output, "OSS: Child %d is terminating at my time %d.%d because it reached %d.%d in user\n", childProcess, **seconds, **nanoseconds, atoi(childSeconds), atoi(childNano));
			//reset the shared memory message buffer to emtpy
			*shmMsg[0] = '\0';
			//wait on the child process that sent 
			//the message to terminate
			if(waitpid(childProcess, status, WUNTRACED) == -1) {
				fprintf(stderr, "terminating child:%d\n", childProcess);
				kill(childProcess, SIGTERM);
			}
			//decrement our number of processes running
			(*currentNumProcesses)--;
			//if we haven't created 100 total
			//processes, fork/exec another one
			if(*totalNumProcesses < 100) {
				(*totalNumProcesses)++;
				createMoreProcesses(*x, *pid, *currentNumProcesses, iMemKey, sMemKey, semKey);
			}

			//if we've reached the total num of
			//processes, increment semaphorex
			//and exit the program
			else if(*totalNumProcesses == 100) {
				cleanup(*seconds, *shmMsg, sem, shmIntId, shmMsgId, semId);
			}
		}

		//if the program needs to keep 
		//running, increment the semaphore
		//and go back through the loop
		sem_post(sem);
	}while(**seconds < 2);
};
