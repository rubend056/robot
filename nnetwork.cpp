#include "nnetwork.h"
#include "img_process.h"

#include "fann.h"

const unsigned int num_input = resx * resy;
const unsigned int num_output = 4;
const unsigned int num_layers = 4;
const float desired_error = (const float) 0.01;
const unsigned int max_epochs = 50000;
const unsigned int epochs_between_reports = 500;

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
			vector<uchar> array;array.reserve(num_input);
			#if DEBUG
			cout << "Image width: " << img_width << endl;
			cout << "Posx: " << obj.x << endl;
			#endif
			array.assign(mat.datastart, mat.dataend);
			for(auto it = array.begin(); it != array.end(); ++it){
				input.push_back (((float)(*it)) / 255);
			}
			
			output[0] = (float)obj.x / img_width;
			output[1] = (float)obj.y / img_height;
			output[2] = (float)obj.w / img_width;
			output[3] = (float)obj.h / img_height;
			
			#if DEBUG
			cout << "Input vector is " << input.size() << " long" << endl;
			cout << "Output array is " << num_output << " long" << endl;
			#endif
		};
		
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
void setCountFile(fs::path cacheDir){
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

void importFile(fs::path cacheDir, fs::path _path, vector<Object> objects){
	getCountFile(cacheDir);
	
	auto mat = cv::imread (_path.string());
	
	cv::Mat trainMat;
	
	trainMat = ip::getTrainMat(mat);
	
	//This function will copy the image to a new location
	cv::imshow("Importing..", writeObjects(mat, objects));
	// #if DEBUG
	// waitKey(30);
	// #else
	waitKey(10);
	// #endif
	
	//Saving file to cache
	imwrite((cacheDir.string() + "/" + Object::getString(objects) + "-" + to_string(import_count++) + ".jpg").c_str(), trainMat);
}
// * --------------------------------------------

vector<TrainData> train_data;

void readFile(fs::path filePath){
	// cout << "Reading " << filePath << endl;
	auto matRaw = cv::imread (filePath.string());
	// cout << "Resizing" << endl;
	Mat mat;resize(matRaw, mat, Size(resx, resy));
	// cout << "Pushing" << endl;
	train_data.push_back(TrainData(mat, Object::getObjects(filePath.filename().string())[0], matRaw.cols, matRaw.rows));
}

void saveTrainData(fs::path savePath){
	ofstream dfile;
	dfile.open(savePath.c_str());
	dfile << train_data.size() << " " << num_input << " " << num_output << endl;
	for(auto it = train_data.begin(); it != train_data.end(); ++it){
		auto input = &(*it).input;
		auto output = (*it).output;
		for(auto dit = input->begin(); dit != input->end(); ++dit){
			if(dit != input->begin())dfile << " ";
			dfile << *dit;
		}
		dfile << endl;
		for(int i = 0; i < num_output; i++){
			if(i != 0)dfile << " ";
			dfile << output[i];
		}
		dfile << endl;
		
	}
	dfile.close();
}

void train(fs::path dataPath, fs::path nn_output){
	struct fann *ann = fann_create_standard(num_layers, num_input, 40, 10, num_output);
	cout << "NUM output" << num_output << endl;
	cout << "NUM output" << ann->num_output << endl;
	
	fann_set_activation_function_hidden(ann, fann_activationfunc_enum::FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(ann, FANN_SIGMOID_SYMMETRIC);
	
	cout << "Fann training from file " << dataPath << endl;
	fann_train_on_file(ann, dataPath.c_str(), max_epochs, epochs_between_reports, desired_error);
	
	cout << "Saving fann to file " << nn_output << endl;
	fann_save(ann, nn_output.c_str());

	fann_destroy(ann);
}