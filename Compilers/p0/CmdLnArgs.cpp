#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <algorithm>

#include "CmdLnArgs.h"

using namespace std;

static void openFileForWriting(ofstream &writeToFilePtr) {
	writeToFilePtr.open("command_line_input");
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
	if(ptr.peek() == ifstream::traits_type::eof()) {
		cout << "File is empty.  No data to read.\n";
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

string processCmdLnArgs(int argc, char* argv[], string &formattedInput) {

	string keyboardInput, fromFileInput;
	ofstream writeToFilePtr;
	ifstream readFromFilePtr;
	ifstream fileInputPtr;

	if(argc > 2) {
		cout << "Error, too many arguments\n";
		exit(EXIT_FAILURE);
	}
	else if(argc == 2) {
		string fileName = appendToFileName(argv);
		fileInputPtr.open(fileName.c_str());
		checkIfEmptyFile(fileInputPtr);
		while(!fileInputPtr.eof()) {
			getline(fileInputPtr, fromFileInput);
			fromFileInput.append(" ");
			replace(fromFileInput.begin(), fromFileInput.end(), '\t', ' ');
			formattedInput.append(fromFileInput);
		}
		fileInputPtr.close();
	}
	else if(argc == 1) {
		openFileForWriting(writeToFilePtr);
		while(!cin.eof()) {
			getline(cin, keyboardInput);
			writeToFilePtr << keyboardInput;
			keyboardInput.append(" ");
			replace(keyboardInput.begin(), keyboardInput.end(), '\t', ' ');
			formattedInput.append(keyboardInput);
		}
		writeToFilePtr.close();
	}
	
	return formattedInput;
};
