#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <memory>

#include "language.h"
#include "token.h"
#include "testScanner.h"
#include "csv_to_vector.h"
#include "CmdLnArgs.h"
#include "filter.h"
#include "scanner.h"

using namespace std;

static string getTK(Token t) {

	string sArr[] = { "idTK", "numTK", "equalTK", "greaterTK", "lessTK", "colonTK", "plusTK", "minusTK", "multiplyTK", "slashTK", "percentTK", "periodTK", "leftParanthesesTK", 
			"rightParanthesesTK", "commaTK", "leftBraceTK", "rightBraceTK", "semi_colonTK", "leftBracketTK", "rightBracketTK", "keyWordTK", "EOFTK" };

	return sArr[t.getTokenID()];
}

int main(int argc, char *argv[]) {
	ifstream fs;
	processCmdLnArgs(argc, argv, &fs);

	char c;
	vector<Token> tokenVector;
	Token token;
	Language l;
	int lineNum = 1, colNum = 0;
	bool flag_isWaitingForNewLine = false;

	testScanner(&tokenVector, &token, l, &c, &fs, &lineNum, &colNum, &flag_isWaitingForNewLine);
	for(Token t : tokenVector) {
		cout << "{ " << getTK(t) << ", " << t.getTokenInstance() << ", " << t.getInputFileLineNumber() << " }" << endl;
	}
	fs.close();
	return 0;
};
