#ifndef SCANNER_H
#define SCANNER_H

#include "language.h"
#include "token.h"

void scanner(Token *token, Language l, std::ifstream *filePtr, char *currentCharacter, int *lineNumber, int *columnNumber, bool *flag_isWaitingForNewLine);

#endif
