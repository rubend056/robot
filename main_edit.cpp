#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include <iostream>

using namespace cv;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_raw_data_folder>" << endl;
}

int imgw, imgh;
int xslider, yslider;
int wslider, hslider;

static void on_trackbar( int, void* )
{
   
}

int main(int argc, char** argv)
{
	if(argc < 2){usage(argv[0]);return 1;} // Check if we at least have 1 argument
	
	auto rawPath = fs::path(argv[1]);
	if(!fs::exists(rawPath) || !fs::is_directory(rawPath)){cout << "The path must exist and it must be a directory" << endl; return 1;}
	
	
	
	
	
	return 0;
}
