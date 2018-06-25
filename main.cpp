#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

Mat rawMat, hsvMat;


VideoCapture cap;

void startCap(){
  cap = VideoCapture(0);

  // Check if camera opened successfully
  if(!cap.isOpened()){
    cout << "Error opening video stream or file" << endl;
  }
}

void getFrame(){
    cap >> rawMat;
    cvtColor(rawMat, hsvMat, CV_RGB2HSV);
}

int main(int argc, char** argv)
{
	
	startCap();
	
	while(waitKey(40) == -1){ //While nothing is pressed
		getFrame();		

		if (rawMat.empty())break;

		imshow( "VideoRaw", rawMat );
		imshow( "VideoHSV", hsvMat );
	}

	cap.release();
	destroyAllWindows();
	
	return 0;
}


