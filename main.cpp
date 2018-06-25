#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv)
{
	Mat image;
	image = imread("logo.png", 1 );
	Mat resized;
	resize(image, resized, Size(400,400));
	
	// namedWindow("Image", WINDOW_AUTOSIZE);/
	imshow("Image", resized);
// 
	VideoCapture vc = VideoCapture(0);
	
	while(waitKey(30) == -1){ //While nothing is pressed
		namedWindow("Video", WINDOW_AUTOSIZE);
		Mat vidbuffer;
		
	}
	waitKey(0);
	
	return 0;
}
