#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

#include "scanner.h"
#include "filter.h"
#include "bnf.h"
#include "tree.h"

using namespace std;

static bool choice_made = false;

struct node *R(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "R";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	if((*token).getTokenID() == 1) {
		tk->ID_value = stoi((*token).getTokenInstance());
		tk->ID_valueSet = true;
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		return tk;
	}
	else if((*token).getTokenID() == 0) {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		return tk;
	}
	else if((*token).getTokenInstance() == "[") {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->left1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		if((*token).getTokenInstance() == "]") {
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			return tk;
		}
		else {
			cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: ']'\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
		cout << "expected token: 'identifier' -or- 'integer' -or- '['\n";
		exit(EXIT_FAILURE);
	}
};

struct node *M(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "M";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	if((*token).getTokenInstance() == "-") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->left1 = M(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
	else {
		tk->left1 = R(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
};

struct node *N(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "N";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	tk->left1 = M(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if((*token).getTokenInstance() == "/") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->right1 = N(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
	else {
		return tk;
	}
};

struct node *A(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "A";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	tk->left1 = N(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if((*token).getTokenInstance() == "*") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->right1 = A(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
	else {
		return tk;
	}
};

struct node *expr(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "EXPR";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	tk->left1 = A(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if((*token).getTokenInstance() == "+") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->right1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
	else if((*token).getTokenInstance() == "-") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->right1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
	else {
		return tk;
	}
};

struct node *RO(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "RO";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	if((*token).getTokenInstance() == "<") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		return tk;
	}
	else if((*token).getTokenInstance() == "=") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenInstance() == "<") {
			tk->ID += " ";
			tk->ID += (*token).getTokenInstance();
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			return tk;
		}
		else if((*token).getTokenInstance() == ">") {
			tk->ID += " ";
			tk->ID += (*token).getTokenInstance();
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			return tk;
		}
		else if((*token).getTokenInstance() == "=") {
			tk->ID += " ";
			tk->ID += (*token).getTokenInstance();
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			return tk;
		}
		else {
			return tk;
		}
	}
	else if((*token).getTokenInstance() == ">") {
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		return tk;
	}
	else {
		cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
		cout << "expected operator token: '<' -or- '=' -or- '>'\n";
		exit(EXIT_FAILURE);
	}
};

struct node *iff(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "IFF";
	node *tk = NULL;
	int ownLevel = level;
	if((*token).getTokenInstance() == "IFF") {
		tk = makeNode(tk, word, ownLevel);
		ownLevel++;
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenInstance() == "[") {
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			tk->left1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			tk->left2 = RO(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			tk->right1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			if((*token).getTokenInstance() == "]") {
				scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				filter2(token);
				tk->right2 = stat(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				choice_made = true;
				return tk;
			}
			else {
				cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
				cout << "expected token: ']'\n";
				exit(EXIT_FAILURE);
			}
		}
		else {
			cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: '['\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *out(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "OUT";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	if((*token).getTokenInstance() == "Output") {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenInstance() == "[") {
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			tk->left1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			if((*token).getTokenInstance() == "]") {
				scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				filter2(token);
				choice_made = true;
				return tk;
			}
			else {
				cout << "error at line: " << *lineNum << "in token: " << (*token).getTokenInstance() << endl;
				cout << "expected token: ']'\n";
				exit(EXIT_FAILURE);
			}
		}
		else {
			cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: '['\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *in(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "IN";
	node *tk = NULL;
	int ownLevel = level;
	if((*token).getTokenInstance() == "Read") {
		tk = makeNode(tk, word, ownLevel);
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenInstance() == "[") {
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			if((*token).getTokenID() == 0) {
				tk->ID = (*token).getTokenInstance();
				scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				filter2(token);
				if((*token).getTokenInstance() == "]") {
					scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
					filter2(token);
					choice_made = true;
					return tk;
				}
				else {
					cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
					cout << "expected token: ']'\n";
					exit(EXIT_FAILURE);
				}
			}
			else {
				cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
				cout << "expected token: 'identifier'\n";
				exit(EXIT_FAILURE);
			}
		}
		else {
			cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: '['\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *mStat(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "MSTAT";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	tk->left1 = stat(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if((*token).getTokenInstance() == ":") {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->right1 = mStat(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		return tk;
	}
	else {
		return tk;
	}
};

struct node *assign(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "ASSIGN";
	node *tk = NULL;
	int ownLevel = level;
	if((*token).getTokenID() == 0) {
		tk = makeNode(tk, word, ownLevel);
		ownLevel++;
		tk->ID = (*token).getTokenInstance();
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenInstance() == "=") {
			tk->ID += " ";
			tk->ID += (*token).getTokenInstance();
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			tk->left1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			choice_made = true;
			return tk;
		}
		else {
			cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: '='\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *Loop(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "LOOP";
	node *tk = NULL;
	int ownLevel = level;
	if((*token).getTokenInstance() == "Loop") {
		tk = makeNode(tk, word, ownLevel);
		ownLevel++;
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenInstance() == "[") {
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			tk->left1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			tk->left2 = RO(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			tk->right1 = expr(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			if((*token).getTokenInstance() == "]") {
				scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				filter2(token);
				tk->right2 = stat(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				choice_made = true;
				return tk;
			}
			else {
				cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
				cout << "expected token: ']'\n";
				exit(EXIT_FAILURE);
			}
		}
		else {
			cout << "error at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: '['\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *stat(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "STAT";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;

	tk->left1 = in(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if(choice_made == false) {
		tk->left1 = out(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	}
	if(choice_made == false) {
		tk->left1 = block(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	}
	if(choice_made == false) {
		tk->left1 = iff(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	}
	if(choice_made == false) {
		tk->left1 = Loop(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	}
	if(choice_made == false) {
		tk->left1 = assign(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	}

	choice_made = false;
	return tk;
};

struct node *stats(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "STATS";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	tk->left1 = stat(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if((*token).getTokenInstance() == ":") {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->right1 = mStat(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	}
	else {
		cout << "error in stats at line: " << *lineNum << " in token: " << (*token).getTokenInstance() << endl;
		cout << "expected either 'statment' -or- token  ':'\n";
		exit(EXIT_FAILURE);
	}
	return tk;
};

struct node *block(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "BLOCK";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	if((*token).getTokenInstance() == "Begin") {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		tk->left1 = vars(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		tk->right1 = stats(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		if((*token).getTokenInstance() == "End") {
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			choice_made = true;
			return tk;
		}
		else {
			cout << "error in block at line: " << *lineNum << " in token " << (*token).getTokenInstance() << endl;
			cout << "expected keyword 'End'\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *vars(int level, Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "VARS";
	node *tk = NULL;
	int ownLevel = level;
	tk = makeNode(tk, word, ownLevel);
	ownLevel++;
	if((*token).getTokenInstance() == "INT") {
		scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
		filter2(token);
		if((*token).getTokenID() == 0) {
			tk->ID = (*token).getTokenInstance();
			scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
			filter2(token);
			if((*token).getTokenID() == 1) {
				tk->ID_value = stoi((*token).getTokenInstance());
				tk->ID_valueSet = true;
				scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				filter2(token);
				tk->left1 = vars(ownLevel, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
				return tk;
			}
			else {
				cout << "error at line: " << *lineNum << " at token: " << (*token).getTokenInstance() << endl;
				cout << "expected token: 'integer'\n";
				exit(EXIT_FAILURE);
			}
		}
		else {
			cout << "error at line: " << *lineNum << " at token: " << (*token).getTokenInstance() << endl;
			cout << "expected token: 'identifier'\n";
			exit(EXIT_FAILURE);
		}
	}
	else {
		return tk;
	}
};

struct node *program(Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	string word = "PROGRAM";
	node *tk = NULL;
	int level = 0;
	tk = makeNode(tk, word, level);
	level++;
	tk->left1 = vars(level, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	tk->right2 = block(level, token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	return tk;
};

struct node *parser(Token *token, Language l, ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine) {
	scanner(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	filter2(token);
	node *tk = NULL;
	tk = program(token, l, filePtr, c, lineNum, colNum, flag_isWaitingForNewLine);
	if((*token).getTokenID() == 21) { 
		return tk;	
	}
	else {
		cout << "error at line: " << *lineNum << " in token " << (*token).getTokenInstance() << endl;
		cout << "expected token 'EOF'\n";
		exit(EXIT_FAILURE);
	}
};
