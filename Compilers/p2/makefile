GPP = g++

CXXFLAGS = -g -w -Wall -std=c++11

target: frontEnd

frontEnd: main.o scanner.o token.o language.o CmdLnArgs.o filter.o bnf.o tree.o
	$(GPP) $(CXXFLAGS) -o frontEnd main.o scanner.o token.o language.o CmdLnArgs.o filter.o bnf.o tree.o

main.o: main.cpp
	$(GPP) $(CXXFLAGS) -c main.cpp

scanner.o: scanner.h

token.o: token.h

language.o: language.h

CmdLnArgs.o: CmdLnArgs.h

filter.o: filter.h

bnf.o: bnf.h

tree.o: node.h tree.h

clean:
	$(RM) frontEnd *.o command_line_input
