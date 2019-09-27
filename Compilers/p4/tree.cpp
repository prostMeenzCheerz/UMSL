#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <cstdlib>
#include <fstream>

#include "node.h"
#include "tree.h"
#include "CmdLnArgs.h"

using namespace std;

static int counter = 0;
static int inOrderIterator = 0;
static int preOrderIterator = 0;
static int postOrderIterator = 0;
static const char* suffix = ".inorder";
static vector<string> expression;
static int tempVar = 0;
static int labelVar = 0;
static string temp;
static string label;
static bool firstRun = false;
static string accVar;
static vector<string> math;
static vector<string> labels;
static vector<string> fLabelV;
static vector<string> lLabelV;
static vector<string> lifetime;

struct node *newNode(string word, int number) {
	struct node *temp = new node;
	temp->key = word;
	temp->level = number;
	temp->ID_valueSet = false;
	temp->left1 = temp->left2 = temp->right1 = temp->right2 = NULL;
	return temp;
};

struct node *makeNode(struct node *node, string word, int number) {
	return node = newNode(word, number);
};

static vector<vector<string> > global;
static vector<string> local;
static int num = 0;
void varCheck(struct node *nodeValue) {
	if(nodeValue != NULL) {
		if((nodeValue->key == "VARS") && (nodeValue->ID.size() > 0)) {
			if(!(find(local.begin(), local.end(), nodeValue->ID) != local.end())) {
				local.push_back(nodeValue->ID);
			}
			else {
				cout << "error: variable '" << nodeValue->ID << "' was already defined in this scope\n";
				exit(EXIT_FAILURE);
			}
		}
		else if(nodeValue->key == "BLOCK") {
			global.push_back(local);
			num++;
			local.clear();
		}
		else if((nodeValue->key == "MSTAT") && (nodeValue->left1->left1 == NULL)) {
			if(!(local.empty())) {
				local.clear();
			}
			else {
				global.erase(global.begin() + num);
				num--;
			}
		}
		else if(((nodeValue->key == "R") && (nodeValue->ID.size() > 0))|| nodeValue->key == "IN" || nodeValue->key == "ASSIGN") {
			if(nodeValue->key == "ASSIGN") {
				string temp;
				for(char c : nodeValue->ID) {
					if(c == ' ') {
						break;
					}
					else {
						temp += c;
					}
				}
				if((find(local.begin(), local.end(), temp) != local.end())) {
				}
				else {	
					bool found = false;
					for(vector<string> v : global) {
						for(string s : v) {
							if(temp == s) {
								found = true;
								break;
							}
						}	
						if(found == true) {
							break;
						}
					}
					if(found == false) {
						cout << "error: variable '" << nodeValue->ID << "' was not defined in this scope \n";
						exit(EXIT_FAILURE);
					}
				}
			}
			else {
				if(find(local.begin(), local.end(), nodeValue->ID) != local.end()) {
				}
				else {
					bool found = false;
					for(vector<string> v : global) {
						for(string s : v) {
							if(nodeValue->ID == s) {
								found = true;
								break;
							}
						}
						if(found == true) {
							break;
						}
					}
					if(found == false) {
						cout << "error variable: '" << nodeValue->ID << "' was not defined in this scope\n";
						exit(EXIT_FAILURE);
					}
				}
			}
		}
		varCheck(nodeValue->left1);
		varCheck(nodeValue->left2);
		varCheck(nodeValue->right1);
		varCheck(nodeValue->right2);
	}
};

static int numPops = 0;
static vector<string> globalStack;
static vector<string> tempVarStack;
static int scopeNum = 0;
void target(struct node *node, ofstream *fp) {
	if(node == NULL) {
		return;
	}
	else {
		string nodeLabel;
		nodeLabel = node->key;
		if(nodeLabel == "PROGRAM") {
			target(node->left1, fp);
			target(node->right1, fp);
			for(int i = 0; i < numPops; i++) {
				*fp << "POP" << endl;
			}
			*fp << "STOP" << endl;
			auto end = lifetime.end();
			for(auto it = lifetime.begin(); it != end; ++it) {
				end = remove(it + 1, end, *it);
			}
			lifetime.erase(end, lifetime.end());
			for(int i = 0; i < lifetime.size(); i++) {
				*fp << lifetime.at(i) << " " << "0" << endl;
			}
			for(int i = 0; i < tempVarStack.size(); i++) {
				*fp << tempVarStack.at(i) << " " << "0" << endl;
			}
		}
		else if(nodeLabel == "BLOCK") {
			int blockNum = globalStack.size();
			target(node->left1, fp);
			target(node->right1, fp);
			for(int i = globalStack.size(); i > blockNum; i--) {
				globalStack.pop_back();
			}
		}
		else if(nodeLabel == "VARS") {
			scopeNum = globalStack.size();
			if(node->ID.size() > 0) {
				globalStack.push_back(node->ID);
				lifetime.push_back(node->ID);
				*fp << "PUSH" << endl;
				numPops++;
			}
		}
		else if(nodeLabel == "EXPR") {
			if(node->right1 != NULL && node->ID == "-") {
				target(node->right1, fp);
				temp = makeTempVar(tempVar++);
				tempVarStack.push_back(temp);
				math.push_back(temp);
				*fp << "STORE " << temp << endl;
				target(node->left1, fp);
				int top = math.size() - 1;
				*fp << "SUB " << math.at(top) << endl;
				math.pop_back();
			}
			else if(node->right1 != NULL && node->ID == "+") {
				target(node->right1, fp);
				temp = makeTempVar(tempVar++);
				tempVarStack.push_back(temp);
				math.push_back(temp);
				*fp << "STORE " << temp << endl;
				target(node->left1, fp);
				int top = math.size() - 1;
				*fp << "ADD " << math.at(top) << endl;
				math.pop_back();
			}
			else {
				target(node->left1, fp);
			}
		}
		else if(nodeLabel == "A") {
			if(node->right1 != NULL && node->ID == "*") {
				target(node->right1, fp);
				temp = makeTempVar(tempVar++);
				tempVarStack.push_back(temp);
				math.push_back(temp);
				*fp << "STORE " << temp << endl;
				target(node->left1, fp);
				int top = math.size() - 1;
				*fp << "MULT " << math.at(top) << endl;
				math.pop_back();
			}
			else {
				target(node->left1, fp);
			}
		}
		else if(nodeLabel == "N") {
			if(node->right1 != NULL && node->ID == "/") {
				target(node->right1, fp);
				temp = makeTempVar(tempVar++);
				tempVarStack.push_back(temp);
				math.push_back(temp);
				*fp << "STORE " << temp << endl;
				target(node->left1, fp);
				int top = math.size() -1 ;
				*fp << "DIV " << temp << endl;
				math.pop_back();
			}
			else {
				target(node->left1, fp);
			}
		}
		else if(nodeLabel == "M") {
			if(node->ID.size() > 0 && node->ID.size() % 2 != 0) {
				target(node->left1, fp);
				*fp << "MULT -1" << endl;
			}
			else {
				target(node->left1, fp);
			}
		}
		else if(nodeLabel == "R") {
			if(node->ID.size() > 0) {
				for(int i = globalStack.size() - 1; i > -1; i--) {
					if(globalStack[i] == node->ID) {
						int location = i;
						*fp << "STACKR " << location << endl;
						break;
					}
				}
			}
			else if(node->ID.size() == 0 ) {
				*fp << "LOAD " << node->ID_value << endl;
			}
			else {
				target(node->left1, fp);
			}
		}
		else if(nodeLabel == "STATS") {
			target(node->left1, fp);
			target(node->right1, fp);
		}
		else if(nodeLabel == "MSTAT") {
			target(node->left1, fp);
			target(node->right1, fp);
		}
		else if(nodeLabel == "STAT") {
			if(node->left1 != NULL) {
				target(node->left1, fp);
			}
			if(node->left2 != NULL) {
				target(node->left2, fp);
			}
			if(node->right1 != NULL) {
				target(node->right1, fp);
			}
			if(node->right2 != NULL) {
				target(node->right2, fp);
			}
		}
		else if(nodeLabel == "IN") {
			int location = 0;
			for(int i = globalStack.size() - 1; i > -1; i--) {
				if(globalStack[i] == node->ID) {
					location = i;
					break;
				}
			}
			*fp << "READ " << globalStack.at(location) << endl;
			*fp << "LOAD " << globalStack.at(location) << endl;
			*fp << "STACKW " << location << endl;
		}
		else if(nodeLabel == "OUT") {
			target(node->left1, fp);
			temp = makeTempVar(tempVar++);
			tempVarStack.push_back(temp);
			*fp << "STORE " << temp << endl;
			*fp << "WRITE " << temp << endl;
		}
		else if(nodeLabel == "IFF") {
			target(node->right1, fp);
			temp = makeTempVar(tempVar++);
			tempVarStack.push_back(temp);
			*fp << "STORE " << temp << endl;
			target(node->left1, fp);
			*fp << "SUB " << temp << endl;
			label = makeLabel(labelVar++);
			labels.push_back(label);
			string name;
			name = node->left2->ID;
			if(name == "<") {
				*fp << "BRZPOS " << label << endl;
			}
			else if(name == "= <" || name == "=<") {
				*fp << "BRPOS " << label << endl;
			}
			else if(name == ">") {
				*fp << "BRZNEG " << label << endl;
			}
			else if(name == "= >" || name == "=>") {
				*fp << "BRNEG " << label << endl;
			}
			else if(name == "= =" || name == "==") {
				*fp << "BRZERO " << label << endl;
			}
			else if(name == "=") {
				*fp << "BRPOS " << label << endl;
				*fp << "BRNEG " << label << endl;
			}
			target(node->right2, fp);
			int top = labels.size() - 1;
			*fp << labels.at(top) << ": NOOP" << endl;
			labels.pop_back();
			
		}
		else if(nodeLabel == "LOOP") {
			string name;
			name = node->left2->ID;
			temp = makeTempVar(tempVar++);
			tempVarStack.push_back(temp);
			string firstLabel;
			firstLabel = makeLabel(labelVar++);
			fLabelV.push_back(firstLabel);
			string lastLabel;
			lastLabel = makeLabel(labelVar++);
			lLabelV.push_back(lastLabel);
			*fp << firstLabel << ": NOOP " << endl;
			target(node->right1, fp);
			*fp << "STORE " << temp << endl;
			target(node->left1, fp);
			*fp << "SUB " << temp << endl;
			if(name == "<") {
				*fp << "BRZPOS " << lastLabel << endl;
			}
			else if(name == "= <" || name == "=<") {
				*fp << "BRPOS " << lastLabel << endl;
			}
			else if(name == ">") {
				*fp << "BRZNEG " << lastLabel << endl;
			}
			else if(name == "= >" || name == "=>") {
				*fp << "BRNEG " << lastLabel << endl;
			}
			else if(name == "= =" || name == "==") {
				*fp << "BRZERO " << lastLabel << endl;
			}
			else if(name == "=") {
				*fp << "BRPOS " << lastLabel << endl;
				*fp << "BRNEG " << lastLabel << endl;
			}
			target(node->right2, fp);
			int fTop = fLabelV.size() - 1;
			*fp << "BR " << fLabelV.at(fTop) << endl;
			fLabelV.pop_back();
			int lTop = lLabelV.size() - 1;
			*fp << lLabelV.at(lTop) << ": NOOP" << endl;
			lLabelV.pop_back();
		}
		else if(nodeLabel == "ASSIGN") {
			target(node->left1, fp);
			int location = 0;
			for(int i = globalStack.size() - 1; i > -1; i--) {
				if(globalStack[i] == node->ID) {
					location = globalStack.size() - i - 1;
					break;
				}
			}
			*fp << "STACKW " << location << endl;
		}
		else {
			target(node->left1, fp);
			target(node->left2, fp);
			target(node->right1, fp);
			target(node->right2, fp);
		}
	}
};




/*
static vector<vector<tuple<string, int>> > globalTuple;
static vector<tuple<string, int> > localTuple;
static vector<string> v;
int scopeCounter = 0;
void makeTarget(struct node *nodeValue) {
	if(nodeValue != NULL) {
		if((nodeValue->key == "VARS") && (nodeValue->ID.size() > 0)) {
			localTuple.push_back(make_tuple(nodeValue->ID, nodeValue->ID_value));
		}
		else if(nodeValue->key == "BLOCK") {
			globalTuple.push_back(localTuple);
			localTuple.clear();
			scopeCounter++;
		}
		else if((nodeValue->key == "MSTAT") && (nodeValue->left1->left1 == NULL)) {
			if(!(localTuple.empty())) {
				localTuple.clear();
			}
			else {
				globalTuple.erase(globalTuple.begin() + scopeCounter);
				scopeCounter--;
			}
		}
		else if(nodeValue->key == "IN") {
			*fp << "READ " << nodeValue->ID << endl;
		}
		else if(nodeValue->key == "OUT") {
			v.clear();
			v = exprHandler(nodeValue->left1);
			firstRun = true;
			writeInstructions(v);
			

			for(int i = scopeCounter - 1; i >= 0; i--) {
				for(int j = globalTuple[i].size() - 1; j >= 0; j--) {
					if(nodeValue->ID == get<0>(globalTuple[i].at(j))) {
						*fp << get<1>(globalTuple[i].at(j)) << endl;
					}
				}
			}
		}
		else if(nodeValue->key == "ASSIGN") {
			v.clear();
			v = exprHandler(nodeValue->left1);
			writeInstructions(v);
		}
		makeTarget(nodeValue->left1);
		makeTarget(nodeValue->left2);
		makeTarget(nodeValue->right1);
		makeTarget(nodeValue->right2);
	}
};

vector<string> exprHandler(struct node *node) {
	if(node->key == "EXPR") {
		if(node->ID == "+") {
			expression = exprHandler(node->left1);
			expression.push_back("+");
			expression = exprHandler(node->right1);
		}
		else if(node->ID == "-") {
			expression = exprHandler(node->left1);
			expression.push_back("-");
			expression = exprHandler(node->right1);
		}
		else if(node->ID.size() == 0) {
			expression = exprHandler(node->left1);
		}
	}
	else if(node->key == "A") {
		if(node->ID == "*") {
			expression = exprHandler(node->left1);
			expression.push_back("*");
			expression = exprHandler(node->right1);
		}
		else if(node->ID.size() == 0) {
			expression = exprHandler(node->left1);
		}
	}
	else if(node->key == "N") {
		if(node->ID == "/") {
			expression = exprHandler(node->left1);
			expression.push_back("/");
			expression = exprHandler(node->right1);
		}
		else if(node->ID.size() == 0) {
			expression = exprHandler(node->left1);
		}
	}
	else if(node->key == "M") {
		if(node->ID == "-") {
			expression.push_back("-");
			expression = exprHandler(node->left1);
		}
		else if(node->ID.size() == 0) {
			expression = exprHandler(node->left1);
		}
	}
	else if(node->key == "R") {
		if(node->left1 == NULL) {
			if(node->ID_valueSet == true) {
				expression.push_back(to_string(node->ID_value));
				return expression;
			}
			else {
				expression.push_back(node->ID);
				return expression;
			}
		}
		else {
			expression = exprHandler(node->left1);
		}
	}
	return expression;
};

void writeInstructions(vector<string> v) {
	if(v.size() == 1) {
		*fp << "WRITE " << v.at(0) << endl;
		return;
	}
	vector<string>::reverse_iterator rit = find(v.rbegin(), v.rend(), "/");
	while(((v.rend() - rit) - 1) > 0) {
		int index = (v.rend() - rit) - 1;
		int previous = index - 1, next = index + 1;
		if(accVar.size() > 0) {
			temp.clear();
			temp = makeTempVar(tempVar++);
			*fp << "STORE " << temp << endl;
		}
		*fp << "DIV " << v.at(next) << endl;
		int num = stoi(v.at(previous)) / stoi(v.at(next));
		v.at(previous) = to_string(num);
		v.erase(v.begin() + index, (v.begin() + next) + 1);
		*fp << endl;
		rit = find(v.rbegin(), v.rend(), "/");
	}
	rit = find(v.rbegin(), v.rend(), "*");
	while(((v.rend() - rit) - 1) > 0) {
		int index = (v.rend() - rit) - 1;
		int previous = index - 1, next = index + 1;
		if(accVar.size() > 0) {
			
	vector<string>::iterator i =  find(v.begin(), v.end(), "-");
	if(i != v.end()) {
		int index = distance(v.begin(), i);
		int next = index + 1;
		int unary = 1;
		while(1) {
			if(next < v.size() && v.at(next) == "-") {
				unary++;
				next++;
			}
			else {
				break;
			}
		}
		if(unary > 1) {
			int posNeg = unary % 2;
			if(posNeg != 0) {
				*fp << "LOAD " << v.at(next) << endl;
				*fp << "MULT -1" << endl;
				*fp << "STORE " << v.at(next) << endl;
				*fp << "WRITE " << v.at(next) << endl;
			}
			else {
				*fp << "WRITE " << v.at(next) << endl;
			}
		}
	}
};
*/
string makeTempVar(int num) {
	string s;
	s += "Temp";
	s += to_string(num);
	return s;
};

string makeLabel(int num) {
	string l;
	l += "L";
	l += to_string(num);
	return l;
};
