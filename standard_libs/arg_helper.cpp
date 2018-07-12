#include "arg_helper.h"

static int arg_c;
static char** arg_v;
static int c_arg = 1;

string getArg(int shift){
	return string(arg_v[c_arg + shift]);
}
string useArg(){
	auto s = getArg();
	c_arg++;
	return s;
}
void setArgC(int val){
	c_arg = val;
}
bool checkArg(int count, bool usag){
	if(arg_c - c_arg - 1 < count){if(usag)usage(arg_v[0]); std::cout << "Checked false on arg " << c_arg << std::endl;return false;} // Check if we at least have 1 command
	return true;
}
void setArgs(int argc, char** argv){
	arg_c = argc;
	arg_v = argv;
}