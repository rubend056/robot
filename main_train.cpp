#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "nnetwork.h"
#include <iostream>

#include "fann.h"

using namespace cv;

static Mat rawMat, hsvMat, finalMat;

Mat getRGB(){return rawMat;}
Mat getHSV(){return hsvMat;}
Mat getHSV(Mat mat){
	Mat hsv;
	cvtColor(mat, hsv, COLOR_BGR2HSV);
	return hsv;
}
void setFinal(Mat mat){finalMat = mat;}


void usage(const char* comm){
	cout << "Usage: " << comm << "  <command>" << endl;
	cout << "	train   <data_filename> <trained_filename> 		data file must be in the root of the nerual_networks directory" << endl;
	cout << "	import   <raw_folder_path>" << endl;
	cout << "	create_data   <data_filaname>				data will be compiled from cache and saved in the nerual_networks" << endl;
	cout << "	clear_cache						this will clear the cache" << endl;
}

int main(int argc, char** argv)
{
	//Set the signal handler for program exit
	signal(SIGINT, intHandler);
	
	if(argc < 2){usage(argv[0]);return 1;} // Check if we at least have 1 command
	
	//Create the cache dir if it doesn't exist
	auto cacheDir = concatPath(fs::current_path(), cache_dir_name);
	if(!exists(cacheDir)){fs::create_directory(cacheDir);}
	auto nnPath = concatPath(fs::current_path(), nn_dir_name);
	if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	//Use commands given as arguments
	string command(argv[1]);
	if(command == "train"){
		if(argc < 4){usage(argv[0]);return 1;} // Check if we at least have 3 commands
		auto data_filename = concatPath(nnPath, argv[2]);
		auto output_filename = concatPath(nnPath, argv[3]);
		
		if(!fs::exists(data_filename)){cout << data_filename << " doesn't exist" << endl;return 1;}
		train(data_filename, output_filename);
	}else if(command == "import"){
		if(argc < 3){usage(argv[0]);return 1;} // Check if we at least have 2 commands
		
		//Populate train cache**************
		fs::path trainPath(argv[2]);
		cout << "Populating cache with data from " << trainPath << endl;
		
		auto rootPaths = listFolder(trainPath);
		for (auto rootit = rootPaths.begin(); rootit != rootPaths.end(); ++rootit){
			
			if(is_directory(*rootit)){
				cout << "Going through dir " << (*rootit).string() << endl;
				auto dirfiles = listFolder(*rootit);
				bool specific = false;
				vector<Object> objects;
				if(!generalFolder ((*rootit).string())){
					cout << "Not a general folder, will use folder name" << endl;
					//Defined specific, now extract Objects from the dir name
					specific = true;
					objects = Object::getObjects((*rootit).filename().string());
					cout << "Folder name object is " << Object::getString(objects) << endl;
				}
				
				for (auto dirit = dirfiles.begin(); dirit != dirfiles.end(); ++dirit){
					if (is_regular_file(*dirit)){
						cout << "	importing " << (*dirit).string() << endl;
						// dirit has file path for all files inside rootit
						// Now import the file
						
						if(!specific)objects = Object::getObjects((*dirit).filename().string());
						importFile(cacheDir, *dirit, objects);
					}
				}
			}else if (is_regular_file(*rootit)){
				cout << "importing " << (*rootit).string() << endl;
				//Pass *rootit to the file parser
				importFile(cacheDir, *rootit, Object::getObjects((*rootit).filename().string()));
			}
		}
		setCountFile(cacheDir);
	}else if(command == "create_data"){
		if(argc < 3){usage(argv[0]);return 1;} // Check if we at least have 2 commands
		
		auto dataPath = concatPath(nnPath, argv[2]);
		
		cout << "Creating data to file " << dataPath << endl;
		
		auto cachePaths = listFolder(cacheDir);
		for (auto it = cachePaths.begin(); it != cachePaths.end(); ++it){
			if((*it).extension() == ".jpg"){
				readFile(*it);
			}
		}
		
		saveTrainData(dataPath);
	}else if(command == "clear_cache"){
		//Clear cache directory
		cout << "Clearing cache" << endl;
		fs::remove_all(cacheDir);
		fs::create_directory(cacheDir);
	}else{
		cout << "Command " << command << " not understood" << endl;
		usage(argv[0]);return 1;
	}
	
	destroyAllWindows();
	
	return 0;
}




