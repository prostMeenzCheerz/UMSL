#ifndef NODE_H
#define NODE_H

#include <vector>

typedef struct node {
	
	bool ID_valueSet;

	std::string key;
	std::string ID;
	int ID_value;
	int level;
	struct node *left1, *left2, *right1, *right2;
};

#endif
