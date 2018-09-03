#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "nnetwork.h"
#include "arg_helper.h"
#include <iostream>
#include <time.h>

#include "fann.h"

using namespace cv;

static Mat rawMat, hsvMat, finalMat;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <command>" << endl;
	cout << "	create_network  <output_filename>" << endl;
	cout << "		-shortcut (will discard hidden layers argument)" << endl;
	cout << "		-hidden <numb> <> <> <>..." << endl;
	cout << "		(any of the edit arguments can be used at the end)" << endl;
	cout << "	train	<input_filename> <data_filename> <output_filename>" << endl;
	// cout << "		-lowest <true, false>" << endl;
	cout << "		-ti 	<test_image_filename (relative to binary)>" << endl;
	cout << "		-de 	<desired_error = " << desired_error << ">" << endl;
	cout << "		Layer Only" << endl;
	cout << "			-me 	<max_epochs = " << max_epochs << ">" << endl;
	cout << "			-ebr 	<epochs_between_reports = " << epochs_between_reports << ">" << endl;
	cout << "					careful with low ebr numbers as it will attempt" << endl;
	cout << "					to save to file lowest mse each report" << endl;
	cout << "		Cascade(Shortcut) Only" << endl;
	cout << "			-mn 	<max_neurons =  " << max_neurons << ">" << endl;
	cout << "			-nbr 	<neurons_between_reports = " << neurons_between_reports << ">" << endl;
	cout << endl;
	cout << "	test	<nn_filename> <data_filename>" << endl;
	cout << "	import  <raw_folder_path> (absolute path)" << endl;
	cout << "	create_data   <data_filaname>" << endl;
	cout << "		-max 	<max_data_number> (if set, data in cache will be randomly picked)" << endl;
	cout << "	clear_cache" << endl;
	cout << "	print	<nn_filename>" << endl;
	cout << "	edit	<nn_filename>" << endl;
	cout << "		-init <data_filename> (uses Widrow + Nguyen’s algorithm, not to be used with cascade)" << endl;
	cout << "		-randomize <min> <max> (not to be used with cascade)" << endl;
	cout << "		-lr 	<learn_rate>" << endl;
	cout << "		-bfl 	<bit_fail_limit>" << endl;
	cout << "		-ta 	<training_algorithm = incremental, batch, rprop or quickprop>" << endl;
	cout << "		-lm 	<learning_momentum>" << endl;
	cout << "		-sf 	<stop_function= mse, bit>" << endl;
	cout << "		-ef 	<error_function= linear, tanh>" << endl;
	cout << "		-qd 	<quickprop_decay>" << endl;
	cout << "		-qmu 	<quickprop_mu>" << endl;
	cout << "		-rif 	<rprop_increase_factor>" << endl;
	cout << "		-rdf 	<rprop_decrease_factor>" << endl;
	cout << "		-rdmin 	<rprop_delta_min>" << endl;
	cout << "		-rdmax 	<rprop_delta_max>" << endl;
	cout << "		-rdzero <rprop_delta_zero>" << endl;
	cout << "		Cascade(shortcut)>" << endl;
	cout << "			-minoe <min_output_epochs>" << endl;
	cout << "			-maxoe <max_output_epochs>" << endl;
	cout << "			-mince <min_candidate_epochs>" << endl;
	cout << "			-maxce <max_candidate_epochs>" << endl;
	cout << "			-ccf <candidate_change_fraction> (used to stop on stagnation)" << endl;
	cout << "			-cse <candidate_stagnation_epochs> (how many epochs on stagnation to end)" << endl;
	cout << "			-cg <candidate_groups>" << endl;
	
	cout << endl;
}
fs::path nnPath;
bool edit(struct fann* ann, string c){
	bool shortcut = fann_get_network_type(ann) == fann_nettype_enum::FANN_NETTYPE_SHORTCUT;
	//PARAMETERS ************************
	if(c == "-lr"){
		float lr = atof(useArg().c_str());
		fann_set_learning_rate(ann, lr);
		cout << "	Learn rate set to " << lr << endl;
	}else if(c == "-bfl"){
		float fl = atof(useArg().c_str());
		fann_set_bit_fail_limit(ann, fl);
		cout << "	Bit fail limit set to " << fl << endl;
	}else if(c == "-ta"){
		auto tas = useArg();
		fann_train_enum ta = FANN_TRAIN_INCREMENTAL;
		if(tas == "incremental")
			ta = FANN_TRAIN_INCREMENTAL;
		else if(tas == "batch")
			ta = FANN_TRAIN_BATCH;
		else if(tas == "rprop")
			ta = FANN_TRAIN_RPROP;
		else if(tas == "quickprop")
			ta = FANN_TRAIN_QUICKPROP;
		else
			cout << "Training algorithm not recognized" << endl;
		
		fann_set_training_algorithm(ann, ta);
		cout << "	Training algorithm set to " << tas << endl;
	}else if(c == "-lm"){
		float fl = atof(useArg().c_str());
		fann_set_learning_momentum(ann, fl);
		cout << "	Learning momentum set to " << fl << endl;
	}else if(c == "-sf"){
		auto tas = useArg();
		fann_stopfunc_enum ta = FANN_STOPFUNC_MSE;
		if(tas == "mse")
			ta = FANN_STOPFUNC_MSE;
		else if(tas == "bit")
			ta = FANN_STOPFUNC_BIT;
		else
			cout << "Stop function not recognized" << endl;
		
		fann_set_train_stop_function(ann, ta);
		cout << "	Training stop function set to " << tas << endl;
	}else if(c == "-ef"){
		auto tas = useArg();
		fann_errorfunc_enum ta = FANN_ERRORFUNC_LINEAR;
		if(tas == "linear")
			ta = FANN_ERRORFUNC_LINEAR;
		else if(tas == "tanh")
			ta = FANN_ERRORFUNC_TANH;
		else
			cout << "Training error function not recognized" << endl;
		
		fann_set_train_error_function(ann, ta);
		cout << "	Training error function set to " << tas << endl;
	}else if(c == "-qd"){
		float fl = atof(useArg().c_str());
		fann_set_quickprop_decay(ann, fl);
		cout << "	Quickprop decay set to " << fl << endl;
	}else if(c == "-qmu"){
		float fl = atof(useArg().c_str());
		fann_set_quickprop_mu(ann, fl);
		cout << "	Quickprop mu set to " << fl << endl;
	}else if(c == "-rif"){
		float fl = atof(useArg().c_str());
		fann_set_rprop_increase_factor(ann, fl);
		cout << "	Rprop increase factor set to " << fl << endl;
	}else if(c == "-rdf"){
		float fl = atof(useArg().c_str());
		fann_set_rprop_decrease_factor(ann, fl);
		cout << "	Rprop decrease factor set to " << fl << endl;
	}else if(c == "-rdmin"){
		float fl = atof(useArg().c_str());
		fann_set_rprop_delta_min(ann, fl);
		cout << "	Rprop delta min set to " << fl << endl;
	}else if(c == "-rdmax"){
		float fl = atof(useArg().c_str());
		fann_set_rprop_delta_max(ann, fl);
		cout << "	Rprop delta max set to " << fl << endl;
	}else if(c == "-rdzero"){
		float fl = atof(useArg().c_str());
		fann_set_rprop_delta_zero(ann, fl);
		cout << "	Rprop delta zero set to " << fl << endl;
	//INITIALIZATION*********************
	}else if(c == "-init"){
		if (shortcut)cout << "Can't init on shortcut(cascade) network" << endl;
		else{
			auto data_path = concatPath(nnPath, useArg());
			struct fann_train_data *train = fann_read_train_from_file(data_path.c_str());
			
			fann_init_weights(ann, train);
			
			cout << "	Initialized with " << data_path << endl;
			fann_destroy_train(train);
		}
	}else if(c == "-randomize"){
		if (shortcut)cout << "Can't init on shortcut(cascade) network" << endl;
		else{
			float min = atof(useArg().c_str());
			float max = atof(useArg().c_str());
			fann_randomize_weights(ann, min, max);
			cout << "	Randomized weights from " << min << " to " << max << endl;
		}
	}else if(c == "-minoe"){
		float fl = atof(useArg().c_str());
		fann_set_cascade_min_out_epochs(ann, fl);
		cout << "	Cascade min output epochs set to " << fl << endl;
	}else if(c == "-maxoe"){
		float fl = atof(useArg().c_str());
		fann_set_cascade_max_out_epochs(ann, fl);
		cout << "	Cascade max output epochs set to " << fl << endl;
	}else if(c == "-mince"){
		int fl = atoi(useArg().c_str());
		fann_set_cascade_min_cand_epochs(ann, fl);
		cout << "	Cascade min candidate epochs set to " << fl << endl;
	}else if(c == "-maxce"){
		int fl = atoi(useArg().c_str());
		fann_set_cascade_max_cand_epochs(ann, fl);
		cout << "	Cascade max candidate epochs set to " << fl << endl;
	}else if(c == "-ccf"){
		float fl = atof(useArg().c_str());
		fann_set_cascade_candidate_change_fraction(ann, fl);
		cout << "	Cascade candidate fraction set to " << fl << endl;
	}else if(c == "-cse"){
		int fl = atoi(useArg().c_str());
		fann_set_cascade_candidate_stagnation_epochs(ann, fl);
		cout << "	Cascade candidate stagnation epochs set to " << fl << endl;
	}else if(c == "-cg"){
		int fl = atoi(useArg().c_str());
		fann_set_cascade_num_candidate_groups(ann, fl);
		cout << "	Cascade number of candidate groups set to " << fl << endl;
	}else
		return false;
	return true;
}

bool saved_lowest = false;
float lowest_mse = 1;
fs::path output_path;

// float last_mse;
// int mse_fail_count;

cv::Mat testMat;

int FANN_API callback(struct fann *ann, struct fann_train_data *train,
							unsigned int max_epochs, unsigned int epochs_between_reports,
							float desired_error, unsigned int epochs)
{
	auto bf = fann_get_bit_fail(ann);
	auto mse = fann_get_MSE(ann);
	printf("Epochs  %8d.  MSE: %.5f. BitFail: %5d\n", epochs, mse, bf);
	
	if(!testMat.empty()){
		auto pMat = ip::processMat(testMat, ip::colors[0]);
		cv::namedWindow("Test", WINDOW_NORMAL);
		writeObjects(testMat, nn::execute(pMat, ann, testMat.cols, testMat.rows));
		cv::imshow("Test", testMat);
		waitKey(1);
	}
	// if (fann_get_train_stop_function(ann) == fann_stopfunc_enum::FANN_STOPFUNC_MSE){
	// 	if(last_mse - mse <= 0)mse_fail_count++;else mse_fail_count = 0;
	// 	if(mse_fail_count > 3)return -1;
	// 	last_mse = mse;
	// }
	if (mse < lowest_mse){
		lowest_mse = mse;
		saved_lowest = true;
		fann_save(ann, output_path.c_str());
	}
	
	Mat tmat(Size(300, 200), CV_8UC1, Scalar(0,0,0));
	cv::imshow("Train", ip::showTraining(tmat, mse));
	waitKey(1);
	
	if(!checkSig())return -1;
	return 0;
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);
	//Set the signal handler for program exit
	signal(SIGINT, intHandler);
	
	srand(time(NULL));
	
	c_arg = 1;
	if(!checkArg())return 1; // Check if we at least have 1 command
	
	//Create the cache dir if it doesn't exist
	auto cacheDir = concatPath(fs::current_path(), cache_dir_name);
	if(!exists(cacheDir)){fs::create_directory(cacheDir);}
	nnPath = concatPath(fs::current_path(), nn_dir_name);
	if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	//Use commands given as arguments
	string command(useArg());
	if(command == "train"){
		if(!checkArg(2))return 1;
		
		auto input_path = concatPath(nnPath, useArg());
		auto data_path = concatPath(nnPath, useArg());
		output_path = concatPath(nnPath, useArg());
		
		cout << endl;
		
		cout << "Training network 	" << input_path << endl;
		cout << "Using data 		" << data_path << endl;
		cout << "Output set to 		" << output_path << endl;
		
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
			}else if(c == "-de"){
				float var = atof(useArg().c_str());
				desired_error = var;
				cout << "	Desired error set to " << var << endl;
			}else if(c == "-ebr"){
				unsigned int var = atoi(useArg().c_str());
				epochs_between_reports = var;
				cout << "	Epochs between reports set to " << var << endl;
			}else if(c == "-ti"){
				auto test_image_filename = concatPath(fs::current_path(), useArg());
				cout << "	Loading test image from " << test_image_filename << endl;
				testMat = imread(test_image_filename.string());
			}
		}
		
		auto ann = nn::ann_load(input_path);
		fann_set_callback(ann, callback); //Set the callback
		
		bool cascade = fann_get_network_type(ann) == fann_nettype_enum::FANN_NETTYPE_SHORTCUT;
		if(cascade){
			cout << "Starting cascade training" << endl;
			fann_cascadetrain_on_file(ann, data_path.c_str(), max_neurons, neurons_between_reports, desired_error);
		}else{
			cout << "Starting layer training" << endl;
			fann_train_on_file(ann, data_path.c_str(), max_epochs, epochs_between_reports, desired_error);
		}
		
		if (saved_lowest){
			cout << "Saved lowest Ann already" << endl;
		}else{
			fann_save(ann, output_path.c_str());
		}
		
		fann_destroy(ann);
	}else if(command == "test"){
		if(!checkArg(1))return 1;
		
		auto input_path = concatPath(nnPath, useArg());
		auto data_path = concatPath(nnPath, useArg());
		
		cout << "Testing network on file " << data_path << endl;
		auto data = fann_read_train_from_file(data_path.c_str());
		cout << "Loading neural network at " << input_path << endl;
		auto ann = nn::ann_load(input_path);
		
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
		
		auto dataPath = concatPath(nnPath, useArg());
		
		cout << "Creating data to file " << dataPath << endl;
		
		int max = 0;
		while(checkArg(0, false)){
			string c(useArg());
			if(c == "-max"){
				int var = atoi(useArg().c_str());
				max = var;
				cout << "	Max set to " << var << endl;
			}
		}
		
		auto cachePaths = listFolder(cacheDir);
		int maxFiles = cachePaths.size();
		
		if(max > 0 && max < maxFiles){
			vector<int> left(cachePaths.size());
			vector<int> toUse;
			for(int i = 0; i<left.size(); i++){
				left[i] = i;
			}
			for(int i = 0; i<left.size(); i++){ // Deleting any file that is not an image
				auto ext = cachePaths[i].extension().string();
				if(ext != ".jpg" || ext != ".jpeg" || ext != ".png")
					left.erase(left.begin() + i);
			}
			for(int i = 0; i<max; i++){
				if(left.size() == 0)break;
				auto rindex = rand() % left.size();
				toUse.push_back(left[rindex]);
				left.erase(left.begin() + rindex);
			}
			
			for(auto it = toUse.begin(); it != toUse.end(); ++it){
				cout << *it << " " << endl;
				nn::readFile(cachePaths[*it]);
			}
		}else{
			for (auto it = cachePaths.begin(); it != cachePaths.end(); ++it){
				auto ext = (*it).extension();
				if(ext == ".jpg" || ext == ".jpeg" || ext == ".png"){
					nn::readFile(*it);
				}
			}
		}
		
		nn::saveTrainData(dataPath);
	}else if(command == "print"){
		if(!checkArg()){return 1;}
		
		auto nn_path = concatPath(nnPath, useArg());
		auto ann = nn::ann_load(nn_path);
		
		fann_print_parameters(ann);
		
		fann_destroy(ann);
	}else if(command == "create_network"){
		if(!checkArg()){return 1;}
		auto output_path = concatPath(nnPath, useArg());
		
		bool shortcut = false;
		vector<unsigned int> hidden;
		hidden.push_back(num_input);
		
		string l;
		while(checkArg(0, false)){
			string c(useArg());
			if(c == "-shortcut"){
				shortcut = true;
				cout << "	Using shortcut(cascade)" << endl;
			}else if(c == "-hidden"){
				int var = atoi(useArg().c_str());
				for(int i = 0; i<var; i++){
					int r = atoi(useArg().c_str());
					hidden.push_back(r);
				}
				cout << "	Hidden layers: " << var << endl;
			}else {l = c;break;}
		}
		hidden.push_back(num_output);
		
		struct fann* ann;
		if(shortcut)
			ann = fann_create_shortcut(2, num_input, num_output);
		else
			ann = fann_create_standard_array(hidden.size(), hidden.data());
		
		edit(ann, l);
		while(checkArg(0, false)){
			auto c = useArg();
			edit(ann, c);
		}
		
		fann_save(ann, output_path.c_str());
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
		
		while(checkArg(0, false)){
			auto c = useArg();
			edit(ann, c);
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




