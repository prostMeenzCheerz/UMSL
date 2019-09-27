#ifndef TREE_H
#define TREE_H

#include "node.h"

struct node *newNode(std::string word, int counter);

struct node *makeNode(struct node *node, std::string word, int counter);

void printPreOrder(struct node *nodeValue);

#endif
