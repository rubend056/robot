#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "nnetwork.h"
#include "arg_helper.h"
#include <iostream>

#include "fann.h"

using namespace cv;

static Mat rawMat, hsvMat, finalMat;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <command>" << endl;
	cout << "	train   <data_filename> <trained_filename>" << endl;
	cout << "			-me <max_epochs = 1000>" << endl;
	cout << "			-de <desired_error = 0.001>" << endl;
	cout << "			-ebr <epochs_between_reports = 50>" << endl;
	cout << "	import   <raw_folder_path>" << endl;
	cout << "	create_data   <data_filaname>" << endl;
	cout << "	clear_cache" << endl;
	cout << "	edit   <trained_filename>" << endl;
	cout << "			-lr <learn_rate>" << endl;
	cout << "			-bfl <bit_fail_limit>" << endl;
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);
	//Set the signal handler for program exit
	signal(SIGINT, intHandler);
	
	// cout << endl;
	
	setArgC(1);
	// cout << getArg() << endl;
	if(!checkArg())return 1; // Check if we at least have 1 command
	
	//Create the cache dir if it doesn't exist
	auto cacheDir = concatPath(fs::current_path(), cache_dir_name);
	if(!exists(cacheDir)){fs::create_directory(cacheDir);}
	auto nnPath = concatPath(fs::current_path(), nn_dir_name);
	if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	//Use commands given as arguments
	string command(useArg());
	if(command == "train"){
		if(!checkArg(2))return 1;
		
		auto data_filename = concatPath(nnPath, useArg());
		auto test_filename = concatPath(nnPath, useArg());
		auto output_filename = concatPath(nnPath, useArg());
		
		cout << endl;
		cout << "Training network from file " << data_filename << endl;
		cout << "Testing network on file 	" << test_filename << endl;
		cout << "Output set to 				" << output_filename << endl;
		
		while(checkArg(1, false)){
			string c(useArg());
			if(c == "-me"){
				unsigned int var = atoi(useArg().c_str());
				max_epochs = var;
				cout << "	Max epochs set to " << var << endl;
			}else if(c == "-de"){
				float var = atof(useArg().c_str());
				desired_error = var;
				cout << "	Desired error set to " << var << endl;
			}else if(c == "-ebr"){
				unsigned int var = atoi(useArg().c_str());
				epochs_between_reports = var;
				cout << "	Epochs between reports set to " << var << endl;
			}
		}
		
		if(!fs::exists(data_filename)){cout << data_filename << " doesn't exist" << endl;return 1;}
		if(!fs::exists(test_filename)){cout << test_filename << " doesn't exist" << endl;return 1;}
		if(!checkArg(0,false)){//We don't have a cached network
			nn::train(data_filename, test_filename, output_filename);
		}else{
			auto cached_nn_path = concatPath(nnPath, useArg());
			cout << "Using network on file " << cached_nn_path << endl;
			if(!fs::exists(cached_nn_path)){cout << cached_nn_path << " doesn't exist" << endl;return 1;}
			nn::train(data_filename, test_filename, output_filename, cached_nn_path);
		}
		
	}else if(command == "import"){
		if(!checkArg())return 1; // Check if we at least have 2 commands
		
		//Populate train cache**************
		fs::path trainPath(useArg());
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
						nn::importFile(cacheDir, *dirit, objects);
					}
				}
			}else if (is_regular_file(*rootit)){
				cout << "importing " << (*rootit).string() << endl;
				//Pass *rootit to the file parser
				nn::importFile(cacheDir, *rootit, Object::getObjects((*rootit).filename().string()));
			}
		}
		nn::setCountFile(cacheDir);
	}else if(command == "create_data"){
		if(!checkArg()){return 1;} // Check if we at least have 2 commands
		
		auto dataPath = concatPath(nnPath, argv[2]);
		
		cout << "Creating data to file " << dataPath << endl;
		
		auto cachePaths = listFolder(cacheDir);
		for (auto it = cachePaths.begin(); it != cachePaths.end(); ++it){
			auto ext = (*it).extension();
			if(ext == ".jpg" || ext == ".jpeg" || ext == ".png"){
				nn::readFile(*it);
			}
		}
		
		nn::saveTrainData(dataPath);
	}else if(command == "clear_cache"){
		//Clear cache directory
		cout << "Clearing cache" << endl;
		fs::remove_all(cacheDir);
		fs::create_directory(cacheDir);
	}else if(command == "edit"){
		if(!checkArg()){return 1;}
		
		auto nn_path = concatPath(nnPath, useArg());
		cout << "Loading neural network at " << nn_path << endl;
		auto ann = nn::ann_load(nn_path);
		
		//Printing all neural network info
		unsigned int layernumb[fann_get_num_layers(ann)];
		fann_get_layer_array(ann, layernumb);
		printf("	Layers: 	");
		for(int i = 0; i<fann_get_num_layers(ann); i++){
			printf("%d	",layernumb[i]);
		}
		cout << endl;
		
		printf ("	Learn Rate: 	%f\n", fann_get_learning_rate(ann));
		printf ("	Bit Fail Limit: %f\n", fann_get_bit_fail_limit(ann));
		cout << endl;
		
		while(checkArg(1, false)){
			string c(useArg());
			if(c == "-lr"){
				float lr = atof(useArg().c_str());
				fann_set_learning_rate(ann, lr);
				cout << "	Learn rate set to " << lr << endl;
			}else if(c == "-bfl"){
				float fl = atof(useArg().c_str());
				fann_set_bit_fail_limit(ann, fl);
				cout << "	Bit fail limit set to " << fl << endl;
			}
		}
		fann_save(ann, nn_path.c_str());
		fann_destroy(ann);
	}else{
		cout << "Command " << command << " not understood" << endl;
		usage(argv[0]);return 1;
	}
	
	destroyAllWindows();
	
	return 0;
}




