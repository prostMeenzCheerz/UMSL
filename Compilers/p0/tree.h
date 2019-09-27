#ifndef TREE_H
#define TREE_H

#include "node.h"

struct node *newNode(std::string key, std::string word);
struct node *makeNode(struct node *node, std::string word);
void buildTree(struct node **word, std::vector<std::string> &tokenizedString);
void printInOrder(struct node *nodeValue, std::ofstream *fp, int argc, char *argv[]);
void printPreOrder(struct node *nodeValue, std::ofstream *fp, int argc, char *argv[]);
void printPostOrder(struct node *nodeValue, std::ofstream *fp, int argc, char *argv[]);

#endif
