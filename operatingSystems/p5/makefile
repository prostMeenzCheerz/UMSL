GCC = gcc
CFLAGS = -g -Wall -lpthread -lrt -lm

all: oss process

oss: oss.o global_functions.o oss_functions.o
	$(GCC) $(CFLAGS) -o oss oss.o global_functions.o oss_functions.o

process: process.o global_functions.o process_functions.o
	$(GCC) $(CFLAGS) -o process process.o global_functions.o process_functions.o

oss.o: oss.c header.h
	$(GCC) $(CFLAGS) -c oss.c

process.o: process.c header.h
	$(GCC) $(CFLAGS) -c process.c

global_functions.o: header.h

oss_functions.o: header.h

process_functions.o: header.h

clean:
	$(RM) oss process *.o
