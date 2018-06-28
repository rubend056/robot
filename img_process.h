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
	
	//Uses main.h function of getRGB and getHSV to get image and process it, dividing it into objects
	vector<Object> process();
	
	//To make an overlay into the given Mat that shows all objects, their identified color and type
	Mat outputProcessed(Mat mat, vector<Object> objects);
	
}

#endif