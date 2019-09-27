#include <iostream>
#include <fstream>
#include <vector>

#include "testScanner.h"

using namespace std;

void testScanner(vector<Token> *tokenVector, Token *token, Language l, char *c, ifstream *filePtr, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	while((*token).getTokenID() != 21) {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		(*tokenVector).push_back(*token);
	}
};
