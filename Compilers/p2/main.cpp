#include <iostream>
#include <fstream>
#include <vector>

#include "language.h"
#include "token.h"
#include "CmdLnArgs.h"
#include "filter.h"
#include "scanner.h"
#include "bnf.h"
#include "tree.h"

using namespace std;

int main(int argc, char *argv[]) {

	ifstream filePtr;
	processCmdLnArgs(argc, argv, &filePtr);
	
	Token token;
	Language language;
	char c;
	int lineNum = 1, colNum = 0;
	bool flag_isWaitingForNewLine = 0;
	node *ptr = NULL;

	ptr = parser(&token, language, &filePtr, &c, &lineNum, &colNum, &flag_isWaitingForNewLine);
	printPreOrder(ptr);

	return 0;
}
