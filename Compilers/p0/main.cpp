#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <vector>
#include <iterator>

#include "node.h"
#include "tree.h"
#include "CmdLnArgs.h"
//#include "stringManip.h"

using namespace std;

static vector<string> putFormattedInputInVector(string &formattedInput);

int main(int argc, char *argv[]) {

	string formattedInput;

	node *word = NULL;

	processCmdLnArgs(argc, argv, formattedInput);

	vector<string> tokenizedString = putFormattedInputInVector(formattedInput);
	buildTree(&word, tokenizedString);

	ofstream inOrderPtr, preOrderPtr, postOrderPtr;	

	printInOrder(word, &inOrderPtr, argc, argv);
	printPreOrder(word, &preOrderPtr, argc, argv);
	printPostOrder(word, &postOrderPtr, argc, argv);
	
	inOrderPtr.close();
	preOrderPtr.close();
	postOrderPtr.close();

	return 0;
}

static vector<string> putFormattedInputInVector(string &formattedInput) {
	istringstream iStreamVar(formattedInput);
	vector<string> tokenizedString((istream_iterator<string>(iStreamVar)), istream_iterator<string>());
	return tokenizedString;
};
