#ifndef MAIN
#define MAIN

#include <cmakeconfig.h>
#include <opencv2/opencv.hpp>

#include "misc.h"

#include <signal.h> // To handle ctl-c exit
void intHandler(int dummy);
bool checkSig();

const string cache_dir_name =			"cache";
const string train_data_dir_name =		"train_data";
const string nn_dir_name = 				"neural_networks";

const auto font_face = cv::FONT_HERSHEY_SIMPLEX;

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
		
		int x = 0, y = 0; //Position of the object in the screen
		int w = 0, h = 0; //The width and height of the object in the screen
		// void shift(float shift){
		// 	x+=shift;
		// 	y+=shift;
		// }
		// void mult(float mult){
		// 	x*=mult;
		// 	y*=mult;
		// 	w*=mult;
		// 	h*=mult;
		// }
		// void div(float div){
		// 	mult(1/div);
		// }
	private:
};

bool generalFolder(string name);

//To make an overlay into the given Mat that shows all objects, their identified color and type
void writeObjects(cv::Mat src, vector<Object> objs);
void writeObjects(cv::Mat src, Object objs);


// cv::Mat getRGB();
// cv::Mat getHSV();
cv::Mat getHSV(cv::Mat mat);
// void setFinal(cv::Mat mat);


#endif