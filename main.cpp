#include <main.h>
#include <img_process.h>
#include <logic_process.h>
#include <iostream>

using namespace cv;



static Mat rawMat, hsvMat, finalMat;
static VideoCapture cap;


void startCap(){
	cap = VideoCapture(0);
	
	// Check if camera opened successfully
	if(!cap.isOpened()){
		cout << "Error opening video stream" << endl;
	}
}

void capture(){
	#ifdef USE_VIDEO
		if(cap.isOpened()){cap >> rawMat;}
	#else
		Mat rawMatRaw = imread("test1.png", 1);
		resize(rawMatRaw, rawMat, Size(711, 400));
	#endif
	
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


