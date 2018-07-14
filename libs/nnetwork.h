#ifndef NNETWORK
#define NNETWORK

#include "main.h"

#include <fann.h>
#include <floatfann.h>

extern unsigned int max_epochs;
extern float desired_error;
extern unsigned int epochs_between_reports;
extern unsigned int max_neurons;
extern unsigned int neurons_between_reports;

namespace nn{

	void importFile(fs::path cacheDir, fs::path _path, vector<Object> objects);
	void readFile(fs::path filePath);

	//Done when finished importing
	void setCountFile(fs::path cacheDir);

	vector<Object> execute(cv::Mat mat);
	Object execute_test_cube_nn(cv::Mat mat, int actual_w, int actual_h, fs::path cubeNNPath);

	struct fann* ann_load(fs::path nn_path);

	void saveTrainData(fs::path cacheDir);
	void train(fs::path dataPath, fs::path testPath, fs::path nn_output);
	void trainCascade(fs::path dataPath, fs::path nn_output);
	void train(fs::path dataPath, fs::path testPath, fs::path nn_output, fs::path nn_file);
	void trainCascade(fs::path dataPath, fs::path nn_output, fs::path nn_file);
	// void setVars(unsigned int me = 100, float de = 0.001f, unsigned int ebr = 100);
}

#endif