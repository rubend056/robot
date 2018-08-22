#include "main.h"
#include "img_process.h"
#include "arg_helper.h"
#include <iostream>

using namespace cv;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_raw_data_folder>" << endl;
}

fs::path current_path;

int main(int argc, char** argv)
{
	setArgs(argc, argv);

	if (!checkArg())return 1;
	auto rawPath = fs::path(useArg());
	if (!fs::exists(rawPath) || !fs::is_directory(rawPath)){cout << "The path must exist and it must be a directory" << endl; return 1;}
	
	Object o;
	
	auto main_list = listFolder(rawPath);
	auto main_c_path = main_list.begin();
	
	while(1){
		switch(waitKey(100)){
			case 27:
				return 0;
				break;
			case 37: // LEFT
				
				break;
			case 39: // RIGHT
				
				break;
			case 38: // UP
				
				break;
			case 40: // DOWN
				
				break;

		}

	}
}
