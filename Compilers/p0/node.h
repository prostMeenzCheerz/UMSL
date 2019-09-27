#ifndef NODE_H
#define NODE_H

#include <vector>

typedef struct node {
	std::string key;
	std::vector<std::string> words;
	int level;
	struct node *left, *right;
};

#endif
