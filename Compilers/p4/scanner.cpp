#include <iostream>
#include <vector>
#include <memory>
#include <fstream>
#include <string>
#include <string.h>
#include <algorithm>
#include <cstdlib>

#include "scanner.h"
#include "filter.h"

using namespace std;

static int enumVal = 0;
static bool flag_firstRun = true, flag_needNewCharacter = true, flag_exitFunction = false;

static Token makeToken(Token &token, int lineNumber, int enumVal) {
	token.setInputFileLineNumber(lineNumber);
	token.setTokenID(enumVal);
	return token;
};

static void clearToken(Token &token) {
	int num = 0;
	token.clearTokenInstance();
	token.setInputFileLineNumber(num);
	token.setTokenID(num);
};

void scanner(Token *token, Language l, ifstream *filePtr, char *currentCharacter, int *lineNumber, int *columnNumber, bool *flag_isWaitingForNewLine) {
	while(flag_firstRun == true) {
		clearToken(*token);
		flag_firstRun = false;
	}
	while(flag_needNewCharacter == true) {
		filter1(filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
		flag_needNewCharacter = false;
	}
	while(!(*filePtr).eof()) {
		int operatorDecValueArr[18] = { 61, 60, 62, 58, 43, 45, 42, 47, 37, 46, 40, 41, 44, 123, 125, 59, 91, 93 };
	
		bool found = false;
		int size = l.getAlphabet().size();
		while(found == false) {
			for(int i = 0; i < size; i++) {
				if(*currentCharacter == l.getAlphabet().at(i)) {
					found = true;
					break;
				}
			}
			break;
		}
		if(found == false) {
			cout << "Error at line " << *lineNumber << endl;
			cout << "'" << *currentCharacter << "' is not valid syntax" << endl;
			exit(EXIT_FAILURE);
		}
	
		if((*token).getTokenInstance().size() == 0) {
			if(*currentCharacter >= 'a' && *currentCharacter <= 'z') {
				cout << "Error at line " << *lineNumber << endl;
				cout << "token cannot start with '" << *currentCharacter << "'" << endl;
				exit(EXIT_FAILURE);
			}
			else if(*currentCharacter == '\n' || *currentCharacter == ' ' || *currentCharacter == '\t') {
				if(*currentCharacter == '\n') {
					(*lineNumber)++;
				}
				flag_firstRun = true;
				flag_needNewCharacter = true;
				return scanner(token, l, filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
			}	
			else {
				(*token).setTokenInstance(*currentCharacter);
				(*token).setInputFileLineNumber(*lineNumber);
				if(*currentCharacter >= 'A' && *currentCharacter <= 'Z') {
					enumVal = 0;
					(*token).setTokenID(enumVal);
					flag_needNewCharacter = true;
					return scanner(token, l, filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
				}
				else if(*currentCharacter >= '0' && *currentCharacter <= '9') {
					enumVal = 1;
					(*token).setTokenID(enumVal);
					flag_needNewCharacter = true;
					return scanner(token, l, filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
				}
				else {
					int size = sizeof(operatorDecValueArr) / sizeof(operatorDecValueArr[0]);
					for(int i = 0; i < size; i++) {
						if(operatorDecValueArr[i] == int(*currentCharacter)) {
							enumVal = (i + 2);
							(*token).setTokenID(enumVal);
							flag_firstRun = true;
							flag_needNewCharacter = true;
							return;
						}
					}
				}
			}
		}
		else if((*token).getTokenInstance().size() > 0) {
			int size = sizeof(operatorDecValueArr) / sizeof(operatorDecValueArr[0]);
			for(int i = 0; i < size; i++) {
				if(operatorDecValueArr[i] == int(*currentCharacter)) {
					flag_firstRun = true;
					flag_needNewCharacter = false;
					return;
				}
			}
			if((*token).getTokenInstance()[0] >= 'A' && (*token).getTokenInstance()[0] <= 'Z') {
				if((*currentCharacter >= 'A' && *currentCharacter <= 'Z') || (*currentCharacter >= 'a' && *currentCharacter <= 'z') || (*currentCharacter >= '0' && *currentCharacter <= '9')) {
					if((*token).getTokenInstance().size() <= 8) {
						(*token).setTokenInstance(*currentCharacter);
						flag_needNewCharacter = true;
						return scanner(token, l, filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
					}
					else {
						flag_firstRun = true;
						flag_needNewCharacter = false;
						return;
					}
				}
				else if(*currentCharacter == ' ' || *currentCharacter == '\t') {
					flag_firstRun = true;
					flag_needNewCharacter = true;
					return;
				}
				else if(*currentCharacter == '\n') {
					(*lineNumber)++;
					flag_firstRun = true;
					flag_needNewCharacter = true;
					return;
				}
			}
			else if((*token).getTokenInstance()[0] >= '0' && (*token).getTokenInstance()[0] <= '9') {
				if(*currentCharacter >= '0' && *currentCharacter <= '9') {
					if((*token).getTokenInstance().size() <= 8) {
						(*token).setTokenInstance(*currentCharacter);
						flag_needNewCharacter = true;
						return scanner(token, l, filePtr, currentCharacter, lineNumber, columnNumber, flag_isWaitingForNewLine);
					}
					else {
						flag_firstRun = true;
						flag_needNewCharacter = false;
						return;
					}
				}
				//new code
				else if((*currentCharacter >= 'A' && *currentCharacter <= 'Z') || (*currentCharacter >= 'a' && *currentCharacter == 'z')) {
					flag_firstRun = true;
					flag_needNewCharacter = false;
					return;
				}
				//end new code
				else if(*currentCharacter == ' ' || *currentCharacter == '\t'){
					flag_firstRun = true;
					flag_needNewCharacter = true;
					return;
				}
				else if(*currentCharacter == '\n') {
					(*lineNumber)++;
					flag_firstRun = true;
					flag_needNewCharacter = true;
					return;
				}	
			}
		}
	}
	char eofArr[] = { 'E', 'O', 'F' };
	for(char c : eofArr) {
		(*token).setTokenInstance(c);
	}
	(*token).setInputFileLineNumber(*lineNumber);
	enumVal = 21;
	(*token).setTokenID(enumVal);
	return;
};
