#ifndef BNF_CPP
#define BNF_CPP

#include "language.h"
#include "token.h"
#include "node.h"

struct node *parser(Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *program(Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *vars(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *block(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *expr(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *A(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *N(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *M(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *F(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *in(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *out(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *iff(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *RO(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *stats(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *stat(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *mStat(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *Loop(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);
struct node *assign(int level, Token *token, Language l, std::ifstream *filePtr, char *c, int *lineNum, int *colNum, bool *flag_isWaitingForNewLine);

#endif
