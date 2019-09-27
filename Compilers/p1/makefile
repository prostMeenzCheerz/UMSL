GPP = g++

CXXFLAGS = -g -w -Wall -std=c++11

target: scanner

scanner: main.o token.o testScanner.o language.o csv_to_vector.o CmdLnArgs.o filter.o scanner.o
	$(GPP) $(CXXFLAGS) -o scanner main.o token.o testScanner.o language.o csv_to_vector.o CmdLnArgs.o filter.o scanner.o

main.o: main.cpp
	$(GPP) $(CXXFLAGS) -c main.cpp

scanner.o: scanner.h

token.o: token.h

testScanner.o: testScanner.h

language.o: language.h

csv_to_vector.o: csv_to_vector.h

CmdLnArgs.o: CmdLnArgs.h

filter.o: filter.h

clean:
	$(RM) scanner *.o command_line_input
