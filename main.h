#ifndef MAIN
#define MAIN

#include <cmakeconfig.h>
#include <opencv2/opencv.hpp>

#include "misc.h"

#include <signal.h> // To handle ctl-c exit
static volatile int keepRunning = 1;
void intHandler(int dummy);

const string cache_dir_name =			"cache";
const string train_data_dir_name =		"train_data";
const string nn_dir_name = 				"neural_networks";

static const int resx = 89, resy = 50;

class Object{
	public:
		Object(){};
		Object(string data){useFilename(data);};
		
		enum Type{Cube, Sphere};
		Type type = Cube;
		
		enum Color{Blue, Green, Red, Yellow};
		Color color = Blue;
		
		string getFileName();
		void useFilename(string name);
		static vector<Object> getObjects(string name);
		static string getString(vector<Object> objects);
		
		int x, y; //Position of the object in the screen
		int w, h; //The width and height of the object in the screen
	private:
};

bool generalFolder(string name);




cv::Mat getRGB();
cv::Mat getHSV();
cv::Mat getHSV(cv::Mat mat);
void setFinal(cv::Mat mat);


#endif