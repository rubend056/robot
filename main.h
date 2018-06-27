#ifndef MAIN
#define MAIN

#include <opencv2/opencv.hpp>
#include <vector>

#define USE_VIDEO

using namespace std;

class Object{
	enum Type{Cube, Sphere};
	Type type = Cube;
	
	enum Color{Blue, Green, Red, Yellow};
	Color color = Blue;
	
	int x, y; //Position of the object in the screen
	int w, h; //The width and height of the object in the screen
};

cv::Mat getRGB();
cv::Mat getHSV();
void setFinal(cv::Mat mat);


#endif