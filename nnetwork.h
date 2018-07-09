#ifndef NNETWORK
#define NNETWORK

#include "main.h"

// const string data_name = "casify.data";
// const string nn_name = "clasify.net";

void importFile(fs::path cacheDir, fs::path _path, vector<Object> objects);
void readFile(fs::path filePath);

//Done when finished importing
void setCountFile(fs::path cacheDir);

void saveTrainData(fs::path cacheDir);
void train(fs::path dataPath, fs::path nn_output);
vector<Object> execute(fs::path fanPath);

#endif