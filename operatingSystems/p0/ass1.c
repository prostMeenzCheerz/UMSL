#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
	
	pid_t childpid = 0; //to hold the value of the child process IDs
	int c, i, n, x, option;
	int hflag = 0, nflag = 0, pflag = 0;  	//used with getopts, option flags are turned on when
						//-h, -n, or -p are used on command line

	char errorMessage[50]; //I'll pass this string to perror
	char *nchars;

	//can't pass variable to perror, so created a string stored in an array using snprintf
	//(to prevent bufferoverflow) so I can use perror and still pass the value held at 'argv[0]'
	snprintf(errorMessage, sizeof errorMessage, "%s: Error: Detailed error message\n", argv[0]);

	while ((option = getopt(argc, argv, "hn:p")) != -1)
		switch (option) {
		case 'h':
			hflag = 1;
			printf("help message\n");
			exit(0);
		case 'n':
			nflag = 1;
			x = atoi(optarg);
			fprintf(stderr, "%d is stored in variable x\n", x);
			exit(0);
		case 'p':
			pflag = 1;
			perror(errorMessage);
			exit(0);
		case '?':
			break;
		default:
			break;
	}
	if (argc != 2) {
		fprintf(stderr, "Usage: %s processes\n", argv[0]);
		return 1;
	}

	n = atoi(argv[1]);
	for(i = 1; i < n; i++) {
		if (childpid = fork()){
		   break;
		}
	}
	fprintf(stderr, "i:%d  process ID:%ld  parent process ID:%ld  child ID:%ld\n", i, (long)getpid(), (long)getppid(), (long)childpid);
	return 0;
}
