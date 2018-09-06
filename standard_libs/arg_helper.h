#ifndef ARG_HELPER
#define ARG_HELPER

#include <string>

extern int c_arg;

std::string getArg(int shift = 0);
std::string useArg();
bool checkArg(int count = 0, bool usag = true);
void setArgs(int argc, char** argv);


// YOU NEED TO IMPLEMENT THIS
void usage(const char* comm);

#endif