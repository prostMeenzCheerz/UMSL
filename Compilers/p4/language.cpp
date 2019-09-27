#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "language.h"

using namespace std;

static fstream streamPtr;

static void fillVectorWithLanguageAlphabet(vector<char> &languageAlphabet) {
	//add uppercase letters A-Z
	for(int i = 65; i < 91; i++) {
		languageAlphabet.push_back(i);
	}
	//add lowercase letters a-z
	for(int i = 97; i < 123; i++) {
		languageAlphabet.push_back(i);
	}
	//add digits 0-9
	for(int i = 48; i < 58; i++) {
		languageAlphabet.push_back(i);
	}
	//add delimiters ( ) * + , - . /
	for(int i = 40; i < 48; i++) {
		languageAlphabet.push_back(i);
	}
	//add delimiters : ; < = >
	for(int i = 58; i < 63; i++) {
		languageAlphabet.push_back(i);
	}
	//add delimiters [ \ ] 
	for(int i = 91; i < 94; i++) {
		languageAlphabet.push_back(i);
	}
	languageAlphabet.push_back(10);  //add newline
	languageAlphabet.push_back(32);  //add delimiter white space
	languageAlphabet.push_back(9); 	 //add tab character
	languageAlphabet.push_back(37);  //add delimiter %
	languageAlphabet.push_back(123); //add delimiter {
	languageAlphabet.push_back(125); //add delimiter }
};

static void fillKeywordVectorWithKeywords(vector<string> &languageKeywords) {
	languageKeywords = { "Begin", "End", "Loop", "Void", "INT", "Return", "Read", "Output", "Program", "IFF", "Then", "Let" };
};

//Token class constructor to instantiate object with alphabet and keyword vectors, and the FSA table
Language::Language() {
	setAlphabet();
	setKeywords();
//	setTable();
};

void Language::setAlphabet() {
	fillVectorWithLanguageAlphabet(alphabet);
};

void Language::setKeywords() {
	fillKeywordVectorWithKeywords(keywords);
};
/*
void Language::setTable() {
	fsaTable = csvFileToVector(&streamPtr);	
};*/
