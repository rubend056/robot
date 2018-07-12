#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "nnetwork.h"

#include "arg_helper.h"

#include <iostream>

using namespace cv;

static VideoCapture cap;
static Mat rawMat, hsvMat, finalMat;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <nn_name> <command>" << endl;
	cout << "	camera   <int camera_numb = 0>" << endl;
	cout << "	image    <path_to_image>" << endl;
}

void startCap(int camera_numb = 0){
	cap = VideoCapture(camera_numb);
	
	// Check if camera opened successfully
	if(!cap.isOpened()){
		cout << "Error opening video stream" << endl;
	}
}

string img_path = "";
vector<Object> objs;
void capture(){
	if(img_path == ""){
		if(cap.isOpened()){cap >> rawMat;}	
	}else if(fs::exists(img_path) && fs::is_regular_file(img_path)){
		rawMat = imread(img_path);
	}else{
		cout << "either image doesn't exist, or it is not a file" << endl;
	}
	if(rawMat.empty())return;
	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV); //Extract the HSV color space from image
}
void process(fs::path nn_path){
	if(rawMat.empty())return;
	auto blueMat = ip::processMat(rawMat, ip::colors[0]);
	objs.clear();
	objs.push_back(nn::execute_test_cube_nn(blueMat,rawMat.rows, rawMat.cols, nn_path));
	finalMat = writeObjects(rawMat, objs);
	cout << objs[0].getFileName() << endl;
}
void display(){
	if(!finalMat.empty())imshow("Final", finalMat);
}

Mat getRGB(){return rawMat;}
Mat getHSV(){return hsvMat;}
void setFinal(Mat mat){finalMat = mat;}

int main(int argc, char** argv)
{
	setArgs(argc, argv);
	bool video = false;
	
	setArgC(1);
	if(!checkArg())return 1;
	
	auto nnPath = concatPath(fs::current_path(), nn_dir_name);
	if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	string nn_name = useArg();
	nnPath = concatPath(nnPath, nn_name.c_str());
	
	auto command = useArg();
	if(command == "camera"){
		if(checkArg())//If there's one more argument
			startCap(getInt(getArg()));
		else
			startCap();
		
		video = true;
	}else if(command == "image"){
		if(!checkArg())return 1;
		
		img_path = useArg();
		
		video = false;
	}else{
		usage(argv[0]);
		return 1;
	}
	
	namedWindow("Final", WINDOW_NORMAL);
	resizeWindow("Final", Size(512,384));
	if(video){
		while(waitKey(30) != 27){ //While ESC not pressed
			capture();
			process(nnPath);
			display();
		}
	}else{
		capture();
		process(nnPath);
		display();
		
		waitKey(0);
	}
	
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	
	return 0;
}




