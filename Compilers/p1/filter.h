#ifndef FILTER_H
#define FILTER_H

#include "token.h"

void filter1(std::ifstream *filePtr, char *currentCharacter, int *lineNumber, int *columnNumber, bool *flag_isWaitingForNewLine);
void filter2(Token *token);

#endif
