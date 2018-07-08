//UNFINISHED FILE WORRING ABOUT TRAINING NOW

#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include <iostream>

using namespace cv;


static VideoCapture cap;
static Mat rawMat, hsvMat, finalMat;

Mat getRGB(){return rawMat;}
Mat getHSV(){return hsvMat;}
void setFinal(Mat mat){finalMat = mat;}

void startCap(){
	cap = VideoCapture(1);
	
	// Check if camera opened successfully
	if(!cap.isOpened()){
		cout << "Error opening video stream" << endl;
	}
}

void capture(){
	if(cap.isOpened()){cap >> rawMat;}

	
	
	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV); //Extract the HSV color space from image
}

void process(){
	vector<Object> objects = ip::process();
}

void display(){
	if(!rawMat.empty())imshow("Raw", rawMat);
	if(!finalMat.empty())imshow("Final", finalMat);
}

Mat getRGB(){return rawMat;}
Mat getHSV(){return hsvMat;}
void setFinal(Mat mat){finalMat = mat;}



int main(int argc, char** argv)
{
	#ifdef USE_VIDEO
		startCap();
		while(waitKey(30) != 27){ //While ESC not pressed
			capture();
			process();
			display();
		}
	#else
		capture();
		process();
		display();
		
		waitKey(0);
	#endif
	
	cap.release();
	destroyAllWindows();
	
	return 0;
}




