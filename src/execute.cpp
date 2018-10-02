#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "input.h"

#include "arg_helper.h"

// using namespace cv;

cv::VideoCapture cap;
cv::Mat rawMat, hsvMat, greyMat, proMat, finalMat;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <nn_name> <command>" << endl;
	// cout << "	camera   <int camera_numb = 0>" << endl;
	// cout << "	image    <path_to_image>" << endl;
}


string img_path = "";
void capture(){
	if(cap.isOpened())cap >> rawMat;	
	if(rawMat.empty())return;
    
    cv::blur(rawMat, rawMat, cv::Size(3,3));
    cvtColor(rawMat, greyMat, COLOR_BGR2GRAY);
    // cv::fastNlMeansDenoising(greyMat, greyMat, float(3), float(3), 5);
	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV); //Extract the HSV color space from image
}

vector<Object> objects;
void process(){
	if(rawMat.empty())return;
    
	
	proMat = ip::processMat(rawMat, ip::colors[2]);
	
	objects.clear();
	
    cv::Mat cannyMat;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    cv::Canny(proMat, cannyMat, 80,120);
    cv::findContours(cannyMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
	cout << contours.size() << endl;
    
    Mat tmat = Mat::zeros(rawMat.size(), CV_8UC1);
    // if (contours.size()>0)
    //     cv::line(tmat, contours[0][0], contours[0][1], Scalar(255));
    
    
    for(auto c : contours){
        auto c0 = c.begin();
        for(auto cn = c.begin() + 1; cn != c.end();cn ++){
            auto c0 = cn - 1;
            cv::line(tmat, *c0, *cn, cv::Scalar(255));
        }
    }
	finalMat = tmat;
}

void display(){
    if(!rawMat.empty())imshow("Raw", rawMat);
	if(!finalMat.empty())imshow("Final", finalMat);
    if(!proMat.empty())imshow("Filter", proMat);
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);
	bool video = false;
	
	// auto nnPath = concatPath(fs::current_path(), nn_dir_name);
	// if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	c_arg = 1;
	// if(!checkArg())return 1;
	// nnPath = concatPath(nnPath, useArg());
	// ann = nn::ann_load(nnPath);
	
	// auto command = useArg();
	// if(command == "camera"){
	// 	// if(checkArg(0,false))//If there's one more argument
	// 	// 	cap = startCamera(getInt(getArg()));
	// 	// else
	// 	// 	cap = startCamera();
	// 	// cout << "Camera opened sucessfully" << endl;
		
	// 	// video = true;
	// }else if(command == "image"){
	// 	if(!checkArg())return 1;
		
	// 	// img_path = useArg();
		
	// 	// video = false;
	// }else{
	// 	usage(argv[0]);
	// 	return 1;
	// }
    cap = startCamera();
    
    bool running = true;
    while(running){ //While ESC not pressed
        int key = waitKey(30);
        switch(key){
            case K_ESC:
                running = false;
                break;
            default:
                handleCameraInput(cap, key);
        }
        capture();
        process();
        display();
    }
	
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	
	return 0;
}




