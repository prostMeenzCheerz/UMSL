#include <sstream>
#include <vector>
#include <iterator>

#include "stringManip.h"
#include "tree.h"

using namespace std;

vector<string> putFormattedInputInVector(string &formattedInput) {
	istringstream iStreamVar(formattedInput);
	vector<string> tokenizedString((istream_iterator<string>(iStreamVar)), istream_iterator<string>());
	return tokenizedString;
};
