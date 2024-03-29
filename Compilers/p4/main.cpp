#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include <string>

#include "language.h"
#include "token.h"
#include "filter.h"
#include "scanner.h"
#include "bnf.h"
#include "tree.h"
#include "CmdLnArgs.h"

using namespace std;

int main(int argc, char *argv[]) {

	ifstream filePtr;
	string name = argv[1];
	name += ".asm";
	ofstream fp;
	if(argc == 2) {
		fp.open(name, ios::out | ios::trunc);
	}
	else if(argc < 2) {
		fp.open("out.asm", ios::out | ios::trunc);
	}
	processCmdLnArgs(argc, argv, &filePtr);
	
	Token token;
	Language language;
	char c;
	int lineNum = 1, colNum = 0;
	bool flag_isWaitingForNewLine = 0;
	node *ptr = NULL;

	ptr = parser(&token, language, &filePtr, &c, &lineNum, &colNum, &flag_isWaitingForNewLine);
	varCheck(ptr);
	target(ptr, &fp);

	return 0;
}
