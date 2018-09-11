/*
	This will handle all image processing that needs to be done, input will be camera buffer, outputting the screen 2d coordinates from a top view of all objects that are seen by the camera. This will be a coputation heavy class and possibly one of the most challenging ones
*/

#ifndef IMG_PROCESS
#define IMG_PROCESS

#include "main.h"
#include "logic_process.h"
#include <vector>

using namespace cv;

namespace ip{
	struct Color{
		public:
			int hue;	// From 0-360
			int range;	// From 0-360
			Color(int h, int r){
				hue = h;
				range = r;
			}
	};

	//Define all the colors
	const vector<Color> colors = {/*BLUE 178-260*/ Color(219, 41), /*GREEN 90-150*/ Color(120, 30), /*RED 0-29 331-360*/ Color(0, 29), /*YELLOW 30-90*/ Color(60, 30)};
	
	//Uses main.h function of getRGB and getHSV to get image and process it, dividing it into objects
	Mat processMat(Mat mat, Color col);
	
	Mat extractColor (Mat hsvMat, Color col);
	
	Mat getTrainMat(Mat mat);
	
	Mat showTraining(Mat mat, float mse);
}

#endif