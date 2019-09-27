#include <iostream>
#include <memory>
#include <fstream>
#include <string>
#include <vector>

#include "token.h"

using namespace std;

TokenID settingEnumValue(int num) {
	TokenID t;
	if(num == 0) {
		t = idTK;
	}
	else if(num == 1) {
		t = numTK;
	}
	else if(num == 2) {
		t = equalTK;
	}
	else if(num == 3) {
		t = lessTK;
	}
	else if(num == 4) {
		t = greaterTK;
	}
	else if(num == 5) {
		t = colonTK;
	}
	else if(num == 6) {
		t = plusTK;
	}
	else if(num == 7) {
		t = minusTK;
	}
	else if(num == 8) {
		t = multiplyTK;
	}
	else if(num == 9) {
		t = slashTK;
	}
	else if(num == 10) {
		t = percentTK;
	}
	else if(num == 11) {
		t = periodTK;
	}
	else if(num == 12) {
		t = leftParanthesesTK;
	}
	else if(num == 13) {
		t = rightParanthesesTK;
	}
	else if(num == 14) {
		t = commaTK;
	}
	else if(num == 15) {
		t = leftBraceTK;
	}
	else if(num == 16) {
		t = rightBraceTK;
	}
	else if(num == 17) {
		t = semi_colonTK;
	}
	else if(num == 18) {
		t = leftBracketTK;
	}
	else if(num == 19) {
		t = rightBracketTK;
	}
	else if(num == 20) {
		t = keyWordTK;
	}
	else if(num == 21) {
		t = EOFTK;
	}
	return t;
};

Token::Token() {
	int lineNumber = 0, enumID = 0;
	char character;

	setInputFileLineNumber(lineNumber);
	setTokenInstance(character);
	setTokenID(enumID);
};

void Token::setInputFileLineNumber(int lineNumber) {
	inputFileLineNumber = lineNumber;
};

void Token::setTokenInstance(char charFromFile) {
	tokenInstance += charFromFile;
};

void Token::setTokenID(int num) {
	enumInstance_tokenID = settingEnumValue(num);
};

void Token::clearTokenInstance() {
	tokenInstance.clear();
};
