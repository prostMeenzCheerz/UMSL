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

struct node *newNode(string word, int counter) {
	struct node *temp = new node;
	temp->key = word;
	temp->level = counter;
	temp->ID_valueSet = false;
	temp->left1 = temp->left2 = temp->right1 = temp->right2 = NULL;
	return temp;
};

struct node *makeNode(struct node *node, string word, int counter) {
	return node = newNode(word, counter);
};

void printPreOrder(struct node *nodeValue) {
	if(nodeValue != NULL) {
		for(int i = 0; i < nodeValue->level; i++) {
			cout << "  ";
		}
		cout << nodeValue->key;
		if(nodeValue->ID.size() > 0) {
			cout << " " << nodeValue->ID;
		}
		if(nodeValue->ID_valueSet == true) {
			cout << " " << nodeValue->ID_value;
		}
		cout << "\n";
		printPreOrder(nodeValue->left1);
		printPreOrder(nodeValue->left2);
		printPreOrder(nodeValue->right1);
		printPreOrder(nodeValue->right2);
	}
};

