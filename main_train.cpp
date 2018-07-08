#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>


using namespace cv;
using namespace boost::filesystem;

static Mat rawMat, hsvMat, finalMat;

Mat getRGB(){return rawMat;}
Mat getHSV(){return hsvMat;}
void setFinal(Mat mat){finalMat = mat;}


// void process(){
// 	vector<Object> objects = ip::process();
// }

// void display(){
// 	if(!rawMat.empty())imshow("Raw", rawMat);
// 	if(!finalMat.empty())imshow("Final", finalMat);
// }

string getExt(string name){
	return name.substr(0, name.find("."));
}

bool generalFolder(string name){ // General folders hold no object data in folder name and all files provide their own data
	return name.find("_") == -1;
}

vector<path> getAll(path _path){
	vector<path> v;
	copy(directory_iterator(_path), directory_iterator(), back_inserter(v));
	sort(v.begin(), v.end());
	return v;
}

//This function is given all files and their expected outputs
void importFile(path cacheDir, path _path, vector<Object> objects){
	cout << "File " << cacheDir.string() << " imported" << endl;
}


int main(int argc, char** argv)
{
	
	Mat rawMatRaw = imread("test3.jpg", 1);
	resize(rawMatRaw, rawMat, Size(resx, resy));
	
	if(argc < 2){
		cout << "You must provide the train directory as an argument, the directory where all images for training are found" << endl;
		return 1;
	}
	//Create the cache directory
	path cacheDir = current_path().append("/cache");
	if(exists(cacheDir)){
		remove_all(cacheDir);
	}else{
		create_directory(cacheDir);
	}
	
	//Populate train cache**************
	path trainPath(argv[1]);
	cout << "Train path is " << trainPath << endl;
	//Iterating through everything on the trainPath
	auto rootPaths = getAll(trainPath);
	for (auto rootit = rootPaths.begin(); rootit != rootPaths.end(); ++rootit){
		if(is_directory(*rootit)){
			auto dirfiles = getAll(*rootit);
			bool specific = false;
			vector<Object> objects;
			if(!generalFolder ((*rootit).filename().string())){
				//Defined specific, now extract Objects from the dir name
				specific = true;
				auto rootpath = *rootit;
				objects = Object::getObjects(rootpath.filename().string());
			}
			
			for (auto dirit = dirfiles.begin(); dirit != dirfiles.end(); ++dirit){
				if (is_regular_file(*dirit)){
					// dirit has file path for all files inside rootit
					// Now import the file
					
					if(!specific)objects = Object::getObjects((*dirit).filename().string());
					importFile(cacheDir, *dirit, objects);
				}
			}
		}else if (is_regular_file(*rootit)){
			cout << boost::format{"Assuming %1 in trainroot has its own data"} % *rootit << endl;
			//Pass *rootit to the file parser
			importFile(cacheDir, *rootit, Object::getObjects((*rootit).filename().string()));
		}
	}
	
	destroyAllWindows();
	
	return 0;
}




