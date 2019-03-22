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

	// proMat = proMats[0];
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


	// Mat tmat = Mat::zeros(rawMat.size(), CV_8UC3);// Create the final mat

	// auto circles = circle_v[smooth_count];
	// // Mat circle_mat;cv::inRange(proMat, Scalar(30), Scalar(255), circle_mat);
	// cv::HoughCircles(proMat, circles, CV_HOUGH_GRADIENT, 2, (double)proMat.rows/16, 100, 85, 6, proMat.rows/4);
	// cout << circles.size() << " ";
	// for( size_t i = 0; i < circles.size(); i++ ) // Print out the circles
	// {
	// 	Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
	// 	int radius = cvRound(circles[i][2]);
	// 	// circle center
	// 	circle( tmat, center, 3, Scalar(255,255,255), -1, 8, 0 );
	// 	// circle outline
	// 	circle( tmat, center, radius, Scalar(255,255,255), 3, 8, 0 );
	// }

	// objects.clear();
	
	
    // cv::Mat cannyMat;
    // vector<vector<Point>> contours;
    // vector<Vec4i> hierarchy;

    // cv::Canny(proMat, cannyMat, 80,120);
	//// cv::dilate(cannyMat, cannyMat, Mat());
    // cv::findContours(cannyMat, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    
	// cout << contours.size() << " ";
    
    
    // if (contours.size()>0)
    //     cv::line(tmat, contours[0][0], contours[0][1], Scalar(255));

    
	// vector<vector<Point>> new_contours;
 //    for(auto c : contours){
	// 	double length = 0;
 //        for(auto cn = c.begin() + 1; cn != c.end();cn ++){
 //            auto c0 = cn - 1;
	// 		Point diff = *c0 - *cn;
	// 		length += cv::sqrt(diff.x*diff.x + diff.y*diff.y);
 //        }
	// 	if (length > 30)new_contours.push_back(c);
 //    }
	// cout << new_contours.size() << " ";
	
	// int color = 0;
	// for(auto c : new_contours){
 //        for(auto cn = c.begin() + 1; cn != c.end();cn ++){
 //            auto c0 = cn - 1;
 //            cv::line(tmat, *c0, *cn, cv::Scalar(colors[color][0], colors[color][1], colors[color][2]));
 //        }
	// 	color ++;if(color==3)color = 0;
 //    }
	// finalMat = tmat;

	// cout << endl;
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




