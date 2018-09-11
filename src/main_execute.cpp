#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "nnetwork.h"
#include "fann.h"

#include "arg_helper.h"

using namespace cv;

VideoCapture cap;
Mat rawMat, hsvMat, finalMat;

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
// vector<Object> objs;
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

int average_count = 30;
vector<Object> objects;
void process(fann* ann){
	if(rawMat.empty())return;
	
	auto blueMat = ip::processMat(rawMat, ip::colors[2]);
	
	Object write_obj;
	auto obj = nn::execute_test_cube_nn(blueMat,rawMat.rows, rawMat.cols, ann);
	objects.push_back(obj);
	while(objects.size() > average_count)objects.erase(objects.begin());
	for(auto sobj: objects){
		write_obj.x += sobj.x / average_count;
		write_obj.y += sobj.y / average_count;
		write_obj.w += sobj.w / average_count;
		write_obj.h += sobj.h / average_count;
	}
	
	writeObjects(rawMat, write_obj);
	
	finalMat = rawMat;
}
void display(){
	if(!finalMat.empty())imshow("Final", finalMat);
	// waitKey(1);
}

Mat getRGB(){return rawMat;}
Mat getHSV(){return hsvMat;}
void setFinal(Mat mat){finalMat = mat;}

// struct fann* ann_load(fs::path nn_path){
// 	if(!fs::exists(nn_path) || !fs::is_regular_file(nn_path)){cout << "either nn_path doesn't exist, or it is not a file" << endl;exit(1);}
// 	struct fann *ann = fann_create_from_file(nn_path.c_str());
// 	return ann;
// }

fann* ann;
int main(int argc, char** argv)
{
	setArgs(argc, argv);
	bool video = false;
	
	auto nnPath = concatPath(fs::current_path(), nn_dir_name);
	if(!exists(nnPath)){fs::create_directory(nnPath);}
	
	c_arg = 1;
	if(!checkArg())return 1;
	nnPath = concatPath(nnPath, useArg());
	ann = nn::ann_load(nnPath);
	
	auto command = useArg();
	if(command == "camera"){
		if(checkArg(0,false))//If there's one more argument
			startCap(getInt(getArg()));
		else
			startCap();
		cout << "Camera opened sucessfully" << endl;
		
		video = true;
	}else if(command == "image"){
		if(!checkArg())return 1;
		
		img_path = useArg();
		
		video = false;
	}else{
		usage(argv[0]);
		return 1;
	}
	
	// namedWindow("Final", WINDOW_NORMAL);
	// resizeWindow("Final", Size(512,384));
	if(video){
		while(waitKey(30) != 27){ //While ESC not pressed
			capture();
			// cout << "Captured" << endl;
			process(ann);
			// cout << "Processed" << endl;
			display();
			// cout << "Displayed" << endl;
		}
	}else{
		capture();
		process(ann);
		display();
		
		waitKey(0);
	}
	
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	fann_destroy(ann);
	
	return 0;
}




