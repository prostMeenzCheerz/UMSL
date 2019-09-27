#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "filter.h"

using namespace std;

static int getColumnNumber(int charNumValue) {
	int column = 0;
	if(charNumValue == 32) {
		column = 1;
	}
	else if((charNumValue >= 65) && (charNumValue <= 90)) {
		column = 2;
	}
	else if((charNumValue >= 97) && (charNumValue <= 122)) {
		column = 3;
	}
	else if((charNumValue >= 48) && (charNumValue <= 57)) {
		column = 4;
	}
	else if(charNumValue == 61) {
		column = 5;
	}
	else if(charNumValue == 60) {
		column = 6;
	}
	else if(charNumValue == 62) {
		column = 7;
	}
	else if(charNumValue == 58) {
		column = 8;
	}
	else if(charNumValue == 43) {
		column = 9;
	}
	else if(charNumValue == 45) {
		column = 10;
	}
	else if(charNumValue == 42) {
		column = 11;
	}
	else if(charNumValue == 47) {
		column = 12;
	}
	else if(charNumValue == 37) {
		column = 13;
	}
	else if(charNumValue == 46) {
		column = 14;
	}
	else if(charNumValue == 40) {
		column = 15;
	}
	else if(charNumValue == 41) {
		column = 16;
	}
	else if(charNumValue == 44) {
		column = 17;
	}
	else if(charNumValue == 123) {
		column = 18;
	}
	else if(charNumValue == 125) {
		column = 19;
	}
	else if(charNumValue == 59) {
		column = 20;
	}
	else if(charNumValue == 91) {
		column = 21;
	}
	else if(charNumValue == 93) {
		column = 22;
	}
	return column;
};

void filter1(ifstream *filePtr, char *currentCharacter, int *lineNumber, int *columnNumber, bool *flag_isWaitingForNewLine) {
	(*filePtr).get(*currentCharacter);
	if(*flag_isWaitingForNewLine == true && *currentCharacter == '\n') {
		*flag_isWaitingForNewLine = false;
		(*lineNumber)++;
		filter1(filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
	}

	if(*currentCharacter == '\\') {
		*flag_isWaitingForNewLine = true;
	}
	while(*flag_isWaitingForNewLine == true) {
		filter1(filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
	}

};

void filter2(Token *token) {
	Language l;
	for(string s : l.getKeywords()) {
		if(s == (*token).getTokenInstance()) {
			int enumVal = 20;
			(*token).setTokenID(enumVal);
		}
	}
};
