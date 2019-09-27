#ifndef TREE_H
#define TREE_H

#include "node.h"

struct node *newNode(std::string word, int counter);

struct node *makeNode(struct node *node, std::string word, int counter);

void varCheck(struct node *nodeValue);
void target(struct node *nodeValue, std::ofstream *fp);
std::vector<std::string> exprHandler(struct node *node);
void writeInstructions(std::vector<std::string> v);
std::string makeTempVar(int counter);
std::string makeLabel(int counter);

#endif
