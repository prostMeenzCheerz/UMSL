#ifndef TESTSCANNER_H
#define TESTSCANNER_H

#include "token.h"
#include "language.h"
#include "scanner.h"
#include "filter.h"

void testScanner(std::vector<Token> *tokenVector, Token *token, Language l, char *c, std::ifstream *filePtr, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);

#endif
