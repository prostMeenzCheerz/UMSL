GCC = gcc

CFLAGS = -p -g -Wall -lpthread -lrt

all: oss user

oss: oss.o global_functions.o oss_functions.o LPqueue.o HPqueue.o
	$(GCC) $(CFLAGS) -o oss oss.o global_functions.o oss_functions.o LPqueue.o HPqueue.o

user: user.o global_functions.o user_functions.o
	$(GCC) $(CFLAGS) -o user user.o global_functions.o user_functions.o

oss1.o: oss.c functions.h
	$(GCC) $(CFLAGS) -c oss.c

user1.o: user1.c functions.h
	$(GCC) $(CFLAGS) -c user.c

global_functions.o: functions.h

oss_functions.o: functions.h

user_functions.o: functions.h

LPqueue.o: functions.h

HPqueue.o: functions.h

clean:
	$(RM) oss user *.o
