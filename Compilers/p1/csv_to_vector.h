#ifndef CSV_T0_VECTOR_H
#define CSV_TO_VECTOR_H

void openFile(std::fstream *fsPtr);
std::vector<std::vector<std::string> > csvFileToVector(std::fstream *fsPtr);

#endif
