#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <errno.h>

#define BUFF_SZ (sizeof(int) * 2)

int main(int argc, char * argv[]) {

	int i;
	int x = atoi(argv[1]);
	
	int milliCounter = 0;
	int secsCounter = 0;

	key_t key;
	key = ftok(".", 'A');
	printf("key in child %d\n", key);

	int shmid = shmget(key, BUFF_SZ, 0777);
	printf("shmid from child %d\n", shmid);
	if (shmid == -1) {
		perror("problem with shmget from child\n");
	}

	int *seconds = (int *)shmat(shmid, NULL, 0);
	printf("value at seconds %p\n", seconds);
	int *milliseconds = seconds + 1;

	int totalTime = (x * 1000000);

	for(i = 0; i < totalTime; i++) {
		if(*milliseconds > 999) {
			*seconds += 1;
			secsCounter++;

			*milliseconds = 0;
			milliCounter = 0;
		}
		else {
			*milliseconds += 1;
			milliCounter++;
		}
	}
	
	printf("child process:%ld incremented the clock by %d second(s) and %d millisecond(s).\n", (long)getpid(), *seconds, *milliseconds);

	//detach from shared memory
	shmdt(seconds);

return 0;
}
