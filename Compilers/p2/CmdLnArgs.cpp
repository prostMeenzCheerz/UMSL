#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

#include "CmdLnArgs.h"

using namespace std;

static string readOnlyFileFromCmdLn = "command_line_input";

static void openFileForWriting(ofstream &writeToFilePtr) {
	writeToFilePtr.open(readOnlyFileFromCmdLn.c_str());
	if(writeToFilePtr.fail()) {
		cout << "Error opening file\n";
		exit(EXIT_FAILURE);
	}
};

static string appendToFileName(char *argv[]) {
	string fileName(argv[1]);
	string suffix(".input1");
	if(fileName.find(suffix) == string::npos) {
		fileName = fileName + suffix;
		return fileName;
	}
	else {
		return fileName;
	}
}

static void checkIfEmptyFile(ifstream &ptr) {
	if(ptr == NULL) {
		cout << "file does not exist\n";
		exit(EXIT_FAILURE);
	}
};

string makeFileNameForOutput(char *argv[]) {
	string inputFileName(argv[1]);
	size_t index = inputFileName.find(".");
	if(index != string::npos) {
		string fileName = inputFileName.substr(0, index);
		return fileName;
	}
	return inputFileName;
};

void processCmdLnArgs(int argc, char* argv[], ifstream *filePtr) {
	string keyboardInput;
	if(argc > 2) {
		cout << "Error, too many arguments\n";
		exit(EXIT_FAILURE);
	}
	else if(argc == 2) {
		string fileName = appendToFileName(argv);
		(*filePtr).open(fileName.c_str());
		checkIfEmptyFile((*filePtr));
		return;
	}
	else if(argc == 1) {
		ofstream writeToFilePtr;
		openFileForWriting(writeToFilePtr);
		while(getline(cin, keyboardInput)) {
			writeToFilePtr << keyboardInput;
			if(!(cin.eof())) {
				writeToFilePtr << "\n";
			}
		}
		writeToFilePtr.close();
		(*filePtr).open(readOnlyFileFromCmdLn.c_str());
		return;
	}
};
