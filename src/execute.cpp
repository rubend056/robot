#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "input.h"
#include "options.h"
#include "comm.h"
#include "arg_helper.h"
#include <cstring>
#include <vector>

using namespace cv;


cv::VideoCapture cap;
cv::Mat rawMat, hsvMat, greyMat, proMat, finalMat;

Comm myComm;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <nn_name> <command>" << endl;
	// cout << "	camera   <int camera_numb = 0>" << endl;
	// cout << "	image    <path_to_image>" << endl;
}



string img_path = "";
void capture(){
	Mat ourRaw;
	if(cap.isOpened())cap >> ourRaw;
	pyrDown(ourRaw, rawMat, Size(ourRaw.cols/2, ourRaw.rows/2));	
	if(rawMat.empty())return;
    
	finalMat = rawMat.clone();

    // cv::blur(rawMat, rawMat, cv::Size(6,6));
    cvtColor(rawMat, greyMat, COLOR_BGR2GRAY);
    // cv::fastNlMeansDenoising(greyMat, greyMat, float(3), float(3), 5);
	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV); //Extract the HSV color space from image
}

// double colors[][3] = 
// 	{
// 		{255, 0, 0},
// 		{0, 255, 0},
// 		{0, 0, 255},
// 	};

// vector<Object> objects;

vector<vector<Point>> squares;
vector<vector<Point>> balls;

// void updateRaw(){
// 	cvtColor(rawMat, greyMat, COLOR_BGR2GRAY);
//     // cv::fastNlMeansDenoising(greyMat, greyMat, float(3), float(3), 5);
// 	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV); //Extract the HSV color space from image
// }
cv::Mat proMats[MAX_COLORS];

int min_distance=45, param1=300, param2=40;
int minRadius=6, maxRadius=70;
int min_sat=70, min_val=115;
void process(){
	if(rawMat.empty())return;
    
	Mat scaled = rawMat;
	// pyrDown(rawMat, scaled, Size(rawMat.cols/2, rawMat.rows/2));
    // pyrUp(scaled, scaled, rawMat.size());
	// int erosion_size = 2;
	// Mat element = getStructuringElement( MORPH_RECT,
    //                    Size( 2*erosion_size + 1, 2*erosion_size+1 ),
    //                    Point( erosion_size, erosion_size ) );
	
	
	
	// ip::find_cubes(rawMat, squares);
	
	 
	for(int i = 0; i < MAX_COLORS; i++)
		proMats[i] = ip::processMat(hsvMat, ip::colors[i]);

	cv::blur(rawMat, rawMat, cv::Size(10,10));

	for(int i = 0; i < MAX_COLORS; i++){
		ip::find_cubes(proMats[i], squares);
        ip::draw_cubes(finalMat, squares, ip::colors_bgr[i]);
	}

	
	// updateRaw();

	for(int i = 0; i < MAX_COLORS; i++){
        ip::find_balls(proMats[i], (double)min_distance, (double)param1, (double)param2, minRadius, maxRadius);
        ip::draw_balls(finalMat, ip::colors_bgr[i]);
	}
}


const char* window_names[] = {"FilterBlue", "FilterGreen", "FilterRed", "FilterYellow"};
void display(){
    // if(!rawMat.empty())imshow("Raw", rawMat);
	if(!finalMat.empty())imshow("Final", finalMat);
	for(int i=0;i<MAX_COLORS;i++)imshow(window_names[i], proMats[i]);
    // imshow("FilterBlue", proMats[0]);
	// imshow("FilterGreen", proMats[1]);
	// imshow("FilterRed", proMats[2]);
	// imshow("FilterYellow", proMats[3]);
}

std::vector<ip::Color> our_colors = {/*BLUE 178-260*/ ip::Color(219, 41), /*GREEN 90-150*/ ip::Color(120, 30), /*RED 0-29 331-360*/ ip::Color(0, 29), /*YELLOW 30-90*/ ip::Color(60, 30)};


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
    cap = startCamera(CAMERA_NUM);

    namedWindow("HoughCircles");
	createTrackbar("Min Distance", "HoughCircles", &min_distance, 100);
	createTrackbar("Param 1", "HoughCircles", &param1, 450);
	createTrackbar("Param 2", "HoughCircles", &param2, 450);
	createTrackbar("Min Radius", "HoughCircles", &minRadius, 400);
	createTrackbar("Max Radius", "HoughCircles", &maxRadius, 400);
	createTrackbar("Min Saturation", "HoughCircles", &min_sat, 255);
	createTrackbar("Min Value", "HoughCircles", &min_val, 255);


	for(int i=0;i<4;i++){
		namedWindow(window_names[i]);
		createTrackbar("Min Saturation", "HoughCircles", &our_colors[i].minS, 255);
		createTrackbar("Min Value", "HoughCircles", &our_colors[i].minV, 255);
	}
	
    bool running = true;
    while(running){ //While ESC not pressed
        int key = waitKey(2);
        switch(key){
            case K_ESC:
                running = false;
                break;
            default:
				if (key == -1)break;
                if (!handleCameraInput(cap, key))cout << "Unsuported: " << key << endl;
        }
        capture();
        process();
		
		
		
		if(ip::circle_v_o != NULL){
			// cout << (float)((*ip::circle_v_o)[0])<< " "<< (float)((*ip::circle_v_o)[1]) << endl;

			float n[3];
			memcpy(n, &(*ip::circle_v_o)[0], 3*sizeof(float)); // Copying to n
			n[0] = n[0] / rawMat.cols;
			n[1] = n[1] / rawMat.rows;
			n[2] = n[2] / 40;

			uint32_t j[3];
			for(int i=0;i<3;i++)j[i]=*((uint32_t*)&n[i]); // Converting to uint32_t
			cout << n[0] << endl;
			myComm.sendToNav(j, 3);
		}
		

        display();
    }
	
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	
	return 0;
}




