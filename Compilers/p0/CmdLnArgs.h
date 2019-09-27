#ifndef CMDLNARGS_H
#define CMDLNARGS_H

std::string makeFileNameForOutput(char *argv[]);
std::string processCmdLnArgs(int argc, char* argv[], std::string &formattedInput);

#endif
