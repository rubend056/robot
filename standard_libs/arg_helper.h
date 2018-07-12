#ifndef ARG_HELPER
#define ARG_HELPER

#include <string>
#include <iostream>
using namespace std;

void usage(const char* comm);

string getArg(int shift = 0);
string useArg();
bool checkArg(int count = 0, bool usag = true);
void setArgs(int argc, char** argv);

void setArgC(int val);

#endif