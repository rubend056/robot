#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "input.h"

#include "arg_helper.h"

using namespace cv;

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
    
    cv::blur(rawMat, rawMat, cv::Size(6,6));
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

void process(){
	if(rawMat.empty())return;
    
	Mat scaled = rawMat;
	// pyrDown(rawMat, scaled, Size(rawMat.cols/2, rawMat.rows/2));
    // pyrUp(scaled, scaled, rawMat.size());
	// int erosion_size = 2;
	// Mat element = getStructuringElement( MORPH_RECT,
    //                    Size( 2*erosion_size + 1, 2*erosion_size+1 ),
    //                    Point( erosion_size, erosion_size ) );
	erode(rawMat, rawMat, NULL, Point(-1,-1), 2);
	dilate(rawMat, rawMat, NULL, Point(-1,-1), 2);
	
	
	
	for(int i = 0; i < ip::colors.size(); i++){
		proMat = ip::processMat(rawMat, ip::colors[i]);
		
		ip::find_cubes(proMat, squares);
        ip::draw_cubes(rawMat, squares, ip::colors_bgr[i]);
		
        ip::find_balls(proMat);
        ip::draw_balls(rawMat, ip::colors_bgr[i]);
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

void display(){
    if(!rawMat.empty())imshow("Raw", rawMat);
	// if(!finalMat.empty())imshow("Final", finalMat);
 //    if(!proMat.empty())imshow("Filter", proMat);
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
    cap = startCamera(0);
    
    bool running = true;
    while(running){ //While ESC not pressed
        int key = waitKey(30);
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
        display();
    }
	
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	
	return 0;
}




