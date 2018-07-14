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
	cout << "		-t 		<test_data_filename>" << endl;
	cout << "		-l 		<trained_filename>" << endl;
	cout << "		-ti 	<test_image_filename (relative to binary)>" << endl;
	cout << "		-me 	<max_epochs = " << max_epochs << ">" << endl;
	cout << "		-de 	<desired_error = " << desired_error << ">" << endl;
	cout << "		-ebr 	<epochs_between_reports = " << epochs_between_reports << ">" << endl;
	cout << endl;
	cout << "		-cascade (if set training will be done with cascade)" << endl;
	cout << "		-mn 	<max_neurons =  " << max_neurons << "> (for cascade only)" << endl;
	cout << "		-nbr 	<neurons_between_reports = " << neurons_between_reports << "> (for cascade only)" << endl;
	cout << endl;
	
	cout << "	test   <data_filename> <trained_filename>" << endl;
	cout << "	import  <raw_folder_path>" << endl;
	cout << "	create_data   <data_filaname>" << endl;
	cout << "	clear_cache" << endl;
	cout << "	edit    <trained_filename>" << endl;
	cout << "		-lr 	<learn_rate>" << endl;
	cout << "		-bfl 	<bit_fail_limit>" << endl;
	cout << endl;
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);
	//Set the signal handler for program exit
	signal(SIGINT, intHandler);
	
	c_arg = 1;
	if(!checkArg())return 1; // Check if we at least have 1 command
	
	//Create the cache dir if it doesn't exist
	auto cacheDir = concatPath(fs::current_path(), cache_dir_name);
	if(!exists(cacheDir)){fs::create_directory(cacheDir);}
	auto nnPath = concatPath(fs::current_path(), nn_dir_name);
	if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	//Use commands given as arguments
	string command(useArg());
	if(command == "train"){
		if(!checkArg(1))return 1;
		
		fs::path test_filename, load_filename;
		auto data_filename = concatPath(nnPath, useArg());
		auto output_filename = concatPath(nnPath, useArg());
		
		cout << endl;
		cout << "Training network from file " << data_filename << endl;
		cout << "Output set to 				" << output_filename << endl;
		
		bool cascade = false;
		while(checkArg(0, false)){
			string c(useArg());
			if(c == "-me"){
				unsigned int var = atoi(useArg().c_str());
				max_epochs = var;
				cout << "	Max epochs set to " << var << endl;
			}else if(c == "-mn"){
				int var = atoi(useArg().c_str());
				max_neurons = var;
				cout << "	Max neurons set to " << var << endl;
			}else if(c == "-nbr"){
				int var = atoi(useArg().c_str());
				neurons_between_reports = var;
				cout << "	Neurons between reports set to " << var << endl;
			}else if(c == "-cascade"){
				cascade = true;
				cout << "	Training with cascade" << endl;
			}else if(c == "-de"){
				float var = atof(useArg().c_str());
				desired_error = var;
				cout << "	Desired error set to " << var << endl;
			}else if(c == "-ebr"){
				unsigned int var = atoi(useArg().c_str());
				epochs_between_reports = var;
				cout << "	Epochs between reports set to " << var << endl;
			}else if(c == "-t"){
				test_filename = concatPath(nnPath, useArg());
				cout << "	Testing network on file 	" << test_filename << endl;
			}else if(c == "-ti"){
				auto test_image_filename = concatPath(fs::current_path(), useArg());
				cout << "	Loading test image from " << test_image_filename << endl;
				testMat = imread(test_image_filename.string());
			}else if(c == "-l"){
				load_filename = concatPath(nnPath, useArg());
				cout << "	Loading network from file 	" << load_filename << endl;
			}else
				useArg();
		}
		
		if(!fs::exists(data_filename)){cout << data_filename << " doesn't exist" << endl;return 1;}
		if(!test_filename.empty() && !fs::exists(test_filename)){cout << test_filename << " doesn't exist" << endl;return 1;}
		if(!load_filename.empty() && !fs::exists(load_filename)){cout << load_filename << " doesn't exist" << endl;return 1;}
		if(!load_filename.empty()){//We have a cached network
			if(cascade)
				nn::trainCascade(data_filename, output_filename, load_filename);
			else
				nn::train(data_filename, test_filename, output_filename, load_filename);
		}else{
			if(cascade)
				nn::trainCascade(data_filename, output_filename);
			else
				nn::train(data_filename, test_filename, output_filename);
		}
	}else if(command == "test"){
		if(!checkArg(1))return 1;
		
		auto data_filename = concatPath(nnPath, useArg());
		auto trained_filename = concatPath(nnPath, useArg());
		
		cout << "Testing network on file " << data_filename << endl;
		auto data = fann_read_train_from_file(data_filename.c_str());
		cout << "Loading neural network at " << trained_filename << endl;
		auto ann = nn::ann_load(trained_filename);
		
		fann_reset_MSE(ann);
		fann_test_data(ann, data);
		auto t_error = fann_get_MSE(ann);
		auto t_b_fail = fann_get_bit_fail(ann);
		printf("Mean Square Error: %f	BitFail: %d\n", t_error, t_b_fail);
		
		fann_destroy_train(data);
		fann_destroy(ann);
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
		
		fann_print_connections(ann);
		fann_print_parameters(ann);
		
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
			}else
				useArg();
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




