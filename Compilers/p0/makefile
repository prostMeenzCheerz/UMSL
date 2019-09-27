GPP = g++

CPPFLAGS = -g -w -Wall

target: P0

P0: main.o tree.o CmdLnArgs.o
	$(GPP) $(CPPFLAGS) -o P0 main.o tree.o CmdLnArgs.o

main.o: main.cpp
	$(GPP) $(CPPFLAGS) -c main.cpp

tree.o: node.h tree.h

CmdLnArgs.o: CmdLnArgs.h

clean: 
	$(RM) P0 *.o *.txt *.inorder *.preorder *.postorder "command_line_input"
