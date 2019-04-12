#include "nnetwork.h"
#include "img_process.h"
#include <iostream>
#include <fstream>

#include <string>

unsigned int max_epochs = 1000;
float desired_error = 0.0001f;
unsigned int epochs_between_reports = 50;
unsigned int max_neurons = 1000;
unsigned int neurons_between_reports = 1;

// void nn::setVars(unsigned int me, float de, unsigned int ebr){
// 	max_epochs = me;
// 	desired_error = de;
// 	epochs_between_reports = ebr;
// }

using namespace nn;
using namespace cv;

class TrainData{
	public:
		vector<float> input;
		float output[num_output];
			/*
			posx
			posy
			width
			height
			*/
		TrainData(Mat mat, Object obj, int img_width, int img_height){
			
			input = getInputs(mat);
			
			output[0] = (float)obj.x / img_width;
			output[1] = (float)obj.y / img_height;
			output[2] = (float)obj.w / img_width;
			output[3] = (float)obj.h / img_height;
			
			#if DEBUG
			cout << "Image width: " << img_width << endl;
			cout << "Posx: " << obj.x << endl;
			cout << mat.type() << endl;
			cout << "Input vector is " << input.size() << " long" << endl;
			cout << "Output array is " << num_output << " long" << endl;
			#endif
			
		};
		static vector<float> getInputs(Mat mat){
			vector<float> out;
			
			MatIterator_<uchar> it, end;
			for( it = mat.begin<uchar>(), end = mat.end<uchar>(); it != end; ++it)
			{
				out.push_back (((float)(*it)) / 255);
			}
			
			return out;
		}
		
};

// * Importing *****************************************
int import_count = 0;
bool tried_count = false;
void getCountFile(fs::path cacheDir){
	fs::path countFPath ((cacheDir.string() + "/cache.txt").c_str());
	//Getting count file
	if(!tried_count){
		if(fs::exists(countFPath)){
			cout << "Reading count from " << countFPath << endl;
			ifstream countf(countFPath.c_str());
			if(countf.is_open()){
				string line;
				getline(countf, line);
				import_count = getInt(line);
				countf.close();
			}
		}
		tried_count = true;
	}
}
void nn::setCountFile(fs::path cacheDir){
	fs::path countFPath ((cacheDir.string() + "/cache.txt").c_str());
	cout << "Writing count to " << countFPath << endl;
	
	ofstream countf(countFPath.c_str());
	if(countf.is_open()){
		countf << import_count << endl;
		countf.close();
	}else
		cout << "Unable to write to counter file in cache :/" << endl;
}

#include <fstream>

void nn::importFile(fs::path cacheDir, fs::path _path, vector<Object> objects){
	getCountFile(cacheDir);
	
	auto mat = cv::imread (_path.string());
	cv::Mat trainMat = ip::getTrainMat(mat); // Process to get train
	
	//Saving file to cache
	save_DB_image(cacheDir.string(), import_count++, objects[0], trainMat);
	// imwrite((cacheDir.string() + "/" + to_string(import_count++) + "-" + Object::getString(objects) + ".jpg").c_str(), trainMat);
}
// * --------------------------------------------

vector<TrainData> train_data;

void nn::readFile(fs::path filePath){
	auto matRaw = cv::imread (filePath.string());
	Mat mat;resize(matRaw, mat, Size(resx, resy), 0, 0, INTER_AREA);
	// cout << matRaw.size() << endl;
	// cout << mat.size() << endl;
	train_data.push_back(TrainData(mat, Object::getObjects(filePath.filename().string())[0], matRaw.cols, matRaw.rows));
}

void nn::saveTrainData(fs::path savePath){
	
	std::ofstream dfile(savePath.c_str());
	dfile << train_data.size() << ' ' << num_input << ' ' << num_output << '\n';
	for(auto it = train_data.begin(); it != train_data.end(); ++it){
		auto input = &(*it).input;
		auto output = (*it).output;
		
		for(auto dit = input->begin(); dit != input->end(); ++dit){
			if(dit != input->begin())dfile << ' ';
			dfile << *dit;
		}
		dfile << '\n';
		for(int i = 0; i < num_output; i++){
			if(i != 0)dfile << ' ';
			dfile << output[i];
		}
		dfile << '\n';
	}
	dfile.close();
}

struct fann* nn::ann_load(fs::path nn_path){
	if(!fs::exists(nn_path) || !fs::is_regular_file(nn_path)){cout << "either nn_path doesn't exist, or it is not a file" << endl;exit(1);}
	return fann_create_from_file(nn_path.c_str());
}

Object nn::execute(Mat mat, struct fann* ann, int actual_w, int actual_h){
	Object o;

	//Resizing image to correct input
	Mat res(mat);
	if(mat.size().area() != ann->num_input){
		if(resx * resy == ann->num_input){
			resize(mat, res, Size(resx, resy));
		}else{
			cout << "Image size is not correct input for the network" << endl;
			return o;
		}
	}
	
	fann_type *calc_out;
	fann_type input[ann->num_input];
	
	//Set input
	auto inputs = TrainData::getInputs(res);
	for(int i = 0;i<inputs.size();i++){
		input[i] = inputs[i];
	}

	calc_out = fann_run(ann, input);
	
	//Collect output
	o.x = calc_out[0] * actual_w;
	o.y = calc_out[1] * actual_h;
	o.w = calc_out[2] * actual_w;
	o.h = calc_out[3] * actual_h;
	
	return o;
}

Object nn::execute_test_cube_nn(Mat mat, int actual_w, int actual_h, fann* ann){
	auto o = execute(mat, ann, actual_w, actual_h);
	
	return o;
}
// vector<Object> nn::execute(Mat mat){
// 	vector<Object> objs;
// 	if(!cube_ann || !sphere_ann){cout << "One of the fanns is null" << endl;return objs;}
// 	if(mat.empty()){cout << "Mat is empty" << endl;return objs;}
// 	if(mat.size().area() != cube_ann->num_input){cout << "Image size not equal cube nn input" << endl;return objs;}
// 	if(mat.size().area() != sphere_ann->num_input){cout << "Image size not equal sphere nn input" << endl;return objs;}
// 	//Now that everything is OK we can start
	
	
// 	return objs;
// }

// int train(struct fann* ann, fs::path dataPath, fs::path testPath){
// 	struct fann_train_data *data = fann_read_train_from_file(dataPath.c_str());
// 	struct fann_train_data *test_data;
	
// 	bool useTest = !testPath.empty();
// 	if(useTest)test_data = fann_read_train_from_file(testPath.c_str());
	
// 	fann_reset_MSE(ann);
	
// 	int rcount = 0;
// 	for(int i = 1 ; i <= max_epochs ; i++) {
// 		auto error = fann_train_epoch(ann, data);
// 		if ( error < desired_error || checkSig() == false) {
// 			printf("Epoch: %d	Mean Square Error: %f	BitFail: %d\n", i, error, fann_get_bit_fail(ann));
// 			if(!useTest){
// 				fann_destroy_train(data);
// 				return 1;
// 			}
// 			fann_reset_MSE(ann);
// 			fann_test_data(ann, test_data);
// 			auto t_error = fann_get_MSE(ann);
// 			auto t_b_fail = fann_get_bit_fail(ann);
// 			printf("EpocT: %d	Mean Square Error: %f	BitFail: %d\n", i, t_error, t_b_fail);
// 			if(t_error < desired_error || checkSig() == false){
// 				fann_destroy_train(test_data);
// 				fann_destroy_train(data);
// 				return 1;
// 			}
// 		}
		
// 		rcount++;
// 		if(rcount == epochs_between_reports){
// 			rcount = 0;
			
// 			printf("Epoch: %d	Mean Square Error: %f	BitFail: %d\n", i, error, fann_get_bit_fail(ann));
// 		}
// 	}
// 	if(useTest)fann_destroy_train(test_data);
// 	fann_destroy_train(data);
// 	return 0;
// }
// void printNN(struct fann* ann){
// 	cout << "Created neural network:" << endl;
// 	unsigned int layernumb[fann_get_num_layers(ann)];
// 	fann_get_layer_array(ann, layernumb);
// 	printf("	Layers: 	");
// 	for(int i = 0; i<fann_get_num_layers(ann); i++){
// 		printf("%d	",layernumb[i]);
// 	}
// 	cout << endl;
	
// 	printf ("	Learn Rate: 	%f\n", fann_get_learning_rate(ann));
// 	printf ("	Bit Fail Limit: %f\n", fann_get_bit_fail_limit(ann));
// 	cout << endl;
// 	fann_print_parameters(ann);
// }

// void setupFann(struct fann* ann){
// 	fann_set_training_algorithm(ann, FANN_TRAIN_RPROP);
// 	fann_set_activation_function_hidden(ann, FANN_SIGMOID_SYMMETRIC);
// 	fann_set_activation_function_output(ann, FANN_LINEAR);
// 	fann_set_bit_fail_limit(ann, bit_fail_limit);
// 	fann_set_learning_rate(ann, learn_rate);
// }
// void nn::train(fs::path dataPath, fs::path testPath, fs::path nn_output){
// 	cout << "Creating standard network" << endl;
// 	struct fann *ann = fann_create_standard(5, num_input, 100, 40, 40, num_output);
// 	setupFann(ann);
// 	//Printing all neural network info
// 	printNN(ann);
	
// 	train(ann, dataPath, testPath);
	
// 	fann_save(ann, nn_output.c_str());
// 	fann_destroy(ann);
// }
// void nn::trainCascade(fs::path dataPath, fs::path nn_output){
// 	cout << "Creating cascade shortcut network" << endl;
// 	struct fann *ann = fann_create_shortcut(2, num_input, num_output);
// 	setupFann(ann);
// 	printNN(ann);
// 	cout << "Max cand epochs " << fann_get_cascade_max_cand_epochs(ann) << endl;
// 	cout << "Min cand epochs " << fann_get_cascade_min_cand_epochs(ann) << endl;
	
// 	// fann_print_connections(ann);
// 	cout << "Starting cascade training" << endl;
	
// 	fann_cascadetrain_on_file(ann, dataPath.c_str(), max_neurons, neurons_between_reports, desired_error);
	
// 	// fann_print_connections(ann);
	
// 	fann_save(ann, nn_output.c_str());
// 	fann_destroy(ann);
// }
// void nn::train(fs::path dataPath, fs::path testPath, fs::path nn_output, fs::path nn_file){
// 	cout << "Reading from file: " << nn_file << endl;
// 	struct fann *ann = fann_create_from_file(nn_file.c_str());
	
// 	train(ann, dataPath, testPath);
	
// 	fann_save(ann, nn_output.c_str());
// 	fann_destroy(ann);
// }
// void nn::trainCascade(fs::path dataPath, fs::path nn_output, fs::path nn_file){
// 	cout << "Reading from file: " << nn_file << endl;
// 	struct fann *ann = fann_create_from_file(nn_file.c_str());
	
// 	// fann_print_connections(ann);
	
// 	fann_cascadetrain_on_file(ann, dataPath.c_str(), max_neurons, neurons_between_reports, desired_error);
	
// 	// fann_print_connections(ann);
	
// 	fann_save(ann, nn_output.c_str());
// 	fann_destroy(ann);
// }