#ifndef CMDLNARGS_H
#define CMDLNARGS_H

std::string makeFileNameForOutput(char *argv[]);
void processCmdLnArgs(int argc, char* argv[], std::ifstream *filePtr);

#endif
