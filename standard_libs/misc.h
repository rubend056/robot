#ifndef MISC
#define MISC

#include <iostream>
#include <vector>
#include <string>
#include <cstring>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>

namespace fs = boost::filesystem;
using namespace std;

int getInt(string string);
string getExt(string name);
fs::path concatPath(fs::path path, string str);
fs::path concatPath(fs::path path, char * str);
vector<fs::path> listFolder(fs::path _path);

#endif