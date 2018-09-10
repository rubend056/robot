#include "img_process.h"
#include "nnetwork.h"

using namespace ip;
using namespace cv;

const int lowerS = 60, upperS = 255;
const int lowerV = 40, upperV = 255;


Mat ip::extractColor (Mat hsvMat, Color col){
	Mat mask;
	if (hsvMat.empty()){cout << "HSV is empty, can't go on" << endl; return mask;}
	
	int hue = col.hue / 2; 		// To make max 180
	int range = col.range / 2; 	// To make max 180
	
	if (hue < range || 180 - hue < range){ // This part basically wraps any value, in case it goes over or lower than 0-180
		int lrange = hue - range, urange = hue + range;
		int llrange = 0, lurange, ulrange, uurange = 180;
		if(lrange < 0){
			lurange = urange;
			ulrange = 180 + lrange; //Since lrange is negative, it works
		}else{
			lurange = 0 + (urange - 180);
			ulrange = lrange;
		}
		
		Scalar llower (llrange,lowerS,lowerV);
		Scalar lupper (lurange,upperS,upperV);
		
		Scalar ulower (ulrange,lowerS,lowerV);
		Scalar uupper (uurange,upperS,upperV);
		
		Mat lmask; inRange(hsvMat, llower, lupper, lmask);
		Mat umask; inRange(hsvMat, ulower, uupper, umask);
		
		mask = lmask | umask;
	}else{
		Scalar lower (hue - range,lowerS,lowerV);
		Scalar upper (hue + range,upperS,upperV);
		
		inRange(hsvMat, lower, upper, mask); // Extract the color
	}
	return mask;
}

Mat relativeColor(Mat hsvMat, Color col){
	std::vector<cv::Mat> hsv_split;cv::split(hsvMat, hsv_split); // Split the hsv image
	Mat outMat(Size(hsvMat.cols, hsvMat.rows), CV_8UC1, Scalar(0,0,0)); // Create the output image
	
	for(int i=0; i<hsvMat.rows; i++){
	    for(int j=0; j<hsvMat.cols; j++){
	        auto hue_val = ((float)hsv_split[0].at<uchar>(i,j)) / 255; // Normalized hue value
	        auto val_val = ((float)hsv_split[2].at<uchar>(i,j)) / 255; // Normalized value
	        
	    }
	}
	
}

Mat ip::processMat(Mat mat, Color col){
	Mat hsv = getHSV(mat);
	Mat defmask = extractColor(hsv, col);
	
	Mat valueMat;
	extractChannel(hsv, valueMat, 2);
	Mat outputMat;
	valueMat.copyTo(outputMat, defmask);
	return outputMat;
}

Mat ip::getTrainMat(Mat mat){
	return processMat(mat, colors[0]);
}

vector<float> mse_list;
float mse_highest = 0;
//This function will return a mat to help users visualize the training, a simple graph.
//This will call drawing functions on opencv
Mat ip::showTraining(Mat mat, float mse){
	if (mse > mse_highest)mse_highest = mse; //Update highest
	mse_list.push_back(mse); // Add to MSE list
	
	auto it = mse_list.begin();
	int mat_w = mat.cols;
	int mat_h = mat.rows;
	float mse_size = 1.f / (mse_list.size()-1);
	
	
	auto p_a = cv::Point2i(0, mat_h - (*it / mse_highest * mat_h));
	it++;
	if (mse_list.size() <= 2){
		Point2i p_b;
		if (mse_list.size() == 1)
			p_b = cv::Point2i(mat_w, p_a.y);
		else{
			p_b = cv::Point2i(mat_w, mat_h - (*it / mse_highest * mat_h));
		}
		cv::line(mat, p_a, p_b, cv::Scalar(255,255,255));
		return mat;
	}
	
	int count = 1;
	auto p_b = cv::Point2i(mse_size * count * mat_w, mat_h - (*it / mse_highest * mat_h));
	cv::line(mat, p_a, p_b, cv::Scalar(255,255,255));
	it++;
	for (; it!= mse_list.end(); it++){
		count ++;
		p_a = cv::Point2i(p_b);
		p_b = cv::Point2i(mse_size * count * mat_w, mat_h - (*it / mse_highest * mat_h));
		cv::line(mat, p_a, p_b, cv::Scalar(255,255,255));
	}
	
	return mat;
}




// vector<Object> ip::process(){ //Use the trained NN to clashify objects
// 	Mat hsv = getHSV();
// 	Mat rgb = getRGB();
// 	//All the calculations done here
	
// 	vector<Mat> colorMasks(colors.size());
// 	for(int i = 0; i<colors.size(); i++){
// 		colorMasks[i] = extractColor(hsv, colors[i]);
// 	}
	
// 	Mat masked;
// 	rgb.copyTo(masked, colorMasks[2]);
// 	setFinal(masked);
	
// 	//This is a placeholder
// 	vector<Object> toReturn(1);
// 	Object test;
// 	toReturn[0] = test;
// 	return toReturn;
// }



