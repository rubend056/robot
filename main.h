#ifndef MAIN
#define MAIN

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

//#define USE_VIDEO

using namespace std;

const int resx = 89, resy = 50;

class Object{
	public:
		Object(){};
		Object(string data){
			useFilename(data);
		};
		
		enum Type{Cube, Sphere};
		Type type = Cube;
		
		enum Color{Blue, Green, Red, Yellow};
		Color color = Blue;
		
		string getFileName();
		void useFilename(string name);
		static vector<Object> getObjects(string name);
		
		int x, y; //Position of the object in the screen
		int w, h; //The width and height of the object in the screen
	private:
};

cv::Mat getRGB();
cv::Mat getHSV();
void setFinal(cv::Mat mat);


#endif