/*
	This will handle all image processing that needs to be done, input will be camera buffer, outputting the screen 2d coordinates from a top view of all objects that are seen by the camera. This will be a coputation heavy class and possibly one of the most challenging ones
*/

#ifndef IMG_PROCESS
#define IMG_PROCESS

#include "main.h"
#include "logic_process.h"
#include <vector>


namespace ip{
	struct Color{
		public:
			int hue;	// From 0-360
			int range;	// From 0-360
			int minS = 100;
			int minV = 70;
			Color(int h, int r){
				hue = h;
				range = r;
			}
			Color(int h, int r, int _minS, int _minV):Color(h,r){minS = _minS; minV = _minV;}

	};

	//Define all the colors
	const vector<Color> colors = {/*BLUE 178-260*/ Color(219, 41), /*GREEN 90-150*/ Color(120, 30), /*RED 0-29 331-360*/ Color(0, 29), /*YELLOW 30-90*/ Color(60, 30)};
	const vector<cv::Scalar> colors_bgr = {/*BLUE*/ cv::Scalar(255, 0, 0), /*GREEN*/ cv::Scalar(0, 255, 0), /*RED*/ cv::Scalar(0, 0, 255), /*YELLOW*/ cv::Scalar(0, 255, 255)};
	
	//Uses main.h function of getRGB and getHSV to get image and process it, dividing it into objects
	cv::Mat processMat(cv::Mat mat, Color col);
	
	cv::Mat extractColor (cv::Mat hsvMat, Color col);
	// cv::Mat extractColor (cv::Mat hsvMat, Color col, int minV, int minS);
	
	cv::Mat getTrainMat(cv::Mat mat);
	
	extern cv::Vec3f* circle_v_o;
	extern int color_num;
	extern vector<cv::Vec3f> circle_s_oa[];
	// cv::Mat showTraining(cv::Mat mat, float mse);

	void find_cubes(const cv::Mat& hsv, vector<vector<cv::Point>>& squares);
	void draw_cubes(cv::Mat& image, const vector<vector<cv::Point>>& squares, cv::Scalar col);

	void find_balls(const cv::Mat& image, double min_dist, double param1, double param2, int minRadius, int maxRadius);
	void draw_balls(cv::Mat& image, cv::Scalar col);
	void draw_all_balls(cv::Mat& image);
}

#endif