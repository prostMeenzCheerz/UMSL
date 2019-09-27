#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
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

struct node *newNode(string key, string word) {
	struct node *temp = new node;
	temp->key = key;
	temp->words.push_back(word);
	temp->level = counter;
	temp->left = temp->right = NULL;
	return temp;
};

struct node *makeNode(struct node *node, string word) {
	
	string key = word.substr(0,1);

	if(node == NULL) return newNode(key, word);

	if(key < node->key) {
		counter++;
		node->left = makeNode(node->left, word);
	}
	else if(key > node->key) {
		counter++;
		node->right = makeNode(node->right, word);
	}
	else if(key == node->key) {
		vector<string>::iterator i;
		i = find(node->words.begin(), node->words.end(), word);
		if(i == node->words.end())
			node->words.push_back(word);
	}
	
	counter = 0;
	return node;
};

void buildTree(struct node **word, vector<string> &tokenizedString) {
	for(int i = 0; i < tokenizedString.size(); i++) {
		*word = makeNode(*word, tokenizedString.at(i));
	}
};

void printInOrder(struct node *nodeValue, ofstream *fp, int argc, char *argv[]) {

	while(inOrderIterator == 0) {
		if(argc == 1) 
			(*fp).open("out.inorder");
		else {
			string fileName = makeFileNameForOutput(argv);
			fileName = fileName + ".inorder";
			(*fp).open(fileName.c_str());
		}

		if((*fp).fail()) {
			cout << "problem opening file.\n";
			exit(EXIT_FAILURE);
		}
		inOrderIterator++;
	}

	if(nodeValue != NULL) {
		printInOrder(nodeValue->left, fp, argc, argv);

		for(int i = 0; i < nodeValue->level; i++) {
			*fp << "  ";
		}
		*fp << nodeValue->key;
		for(int i = 0; i < nodeValue->words.size(); i++) {
			*fp << " " << nodeValue->words.at(i);
		}
		*fp << "\n";
		printInOrder(nodeValue->right, fp, argc, argv);
	}
};

void printPreOrder(struct node *nodeValue, ofstream *fp, int argc, char *argv[]) {
	while(preOrderIterator == 0) {
		if(argc == 1)
			(*fp).open("out.preorder");
		else {
			string fileName = makeFileNameForOutput(argv);
			fileName = fileName + ".preorder";
			(*fp).open(fileName.c_str());
		}
		if((*fp).fail()) {
			cout << "problem opening file.\n";
			exit(EXIT_FAILURE);
		}
		preOrderIterator++;
	}

	if(nodeValue != NULL) {
		for(int i = 0; i < nodeValue->level; i++) {
			*fp << "  ";
		}
		*fp  << nodeValue->key;
		for(int i = 0; i < nodeValue->words.size(); i++) {
			*fp << " " << nodeValue->words.at(i);
		}
		*fp << "\n";
		printPreOrder(nodeValue->left, fp, argc, argv);
		printPreOrder(nodeValue->right, fp, argc, argv);
	}
};

void printPostOrder(struct node *nodeValue, ofstream *fp, int argc, char *argv[]) {
	while(postOrderIterator == 0) {
		if(argc == 1)
			(*fp).open("out.postorder");
		else {
			string fileName = makeFileNameForOutput(argv);
			fileName = fileName + ".postorder";
			(*fp).open(fileName.c_str());
		}
		if((*fp).fail()) {
			cout << "problem opening file.\n";
			exit(EXIT_FAILURE);
		}
		postOrderIterator++;
	}

	if(nodeValue != NULL) {
		printPostOrder(nodeValue->left, fp, argc, argv);
		printPostOrder(nodeValue->right, fp, argc, argv);
		for(int i = 0; i < nodeValue->level; i++) {
			*fp << "  ";
		}
		*fp << nodeValue->key;
		for(int i = 0; i < nodeValue->words.size(); i++) {
			*fp << " " << nodeValue->words.at(i);
		}
		*fp << "\n";
	}
};
