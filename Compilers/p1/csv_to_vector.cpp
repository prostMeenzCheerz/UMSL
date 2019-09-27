#include <iostream>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <string>

#include "csv_to_vector.h"

using namespace std;

static fstream streamPtr;

void openFile(fstream *fsPtr) {
	(*fsPtr).open("FSA_Table.csv", fstream::in | fstream::out | fstream::app);
	if((*fsPtr).fail()) {
		cout << "trouble opening csv file\n";
		exit(EXIT_FAILURE);
	}
	
};

vector<vector<string> > csvFileToVector(fstream *fsPtr) {
	vector<vector<string> > vectorFromCsv;
	openFile(fsPtr);
	string word;
	int i = 0;

	while(!(*fsPtr).eof()) {
		vector<string> row;
		getline((*fsPtr), word, '|');
		row.push_back(word);
		vectorFromCsv.push_back(row);
		i++;
	}

	return vectorFromCsv;
};
