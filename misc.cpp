#include "misc.h"

fs::path concatPath(fs::path path, string str){
	fs::path p((path.string() + "/" + str).c_str());
	return p;
}
fs::path concatPath(fs::path path, char * str){
	return concatPath(path, string(str));
}

string getExt(string name){
	int index = name.find(".");
	if(index >= 0)
		return name.substr(index, name.length() - index);
	else 
		return "";
}

int getInt(string string){
	return atoi(string.c_str());
}

vector<fs::path> listFolder(fs::path _path){
	vector<fs::path> v;
	copy(fs::directory_iterator(_path), fs::directory_iterator(), back_inserter(v));
	sort(v.begin(), v.end());
	return v;
}