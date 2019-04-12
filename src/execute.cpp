#include "main.h"
#include "img_process.h"
#include "logic_process.h"
#include "comm_class.h"
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

std::vector<ip::Color> our_colors = {/*BLUE 178-260*/ ip::Color(219, 41, 95, 38), /*GREEN 90-150*/ ip::Color(120, 30, 48, 52), /*RED 0-29 331-360*/ ip::Color(0, 29, 68, 79), /*YELLOW 30-90*/ ip::Color(60, 30, 78, 110)};


string img_path = "";
void capture(){
	// Mat ourRaw;
	if(cap.isOpened())cap >> rawMat;
	if(rawMat.empty())return;
	resize(rawMat, rawMat, Size(rawMat.cols/4, rawMat.rows/4));	
	// pyrDown(ourRaw, rawMat, Size(ourRaw.cols/2, ourRaw.rows/2));
	
    
	finalMat = rawMat.clone();

    // cv::blur(rawMat, rawMat, cv::Size(6,6));
    // cvtColor(rawMat, greyMat, COLOR_BGR2GRAY);
    cv::fastNlMeansDenoising(rawMat, rawMat, float(3), float(3), 5);
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
vector<vector<Vec3f>> balls;

// void updateRaw(){
// 	cvtColor(rawMat, greyMat, COLOR_BGR2GRAY);
//     // cv::fastNlMeansDenoising(greyMat, greyMat, float(3), float(3), 5);
// 	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV); //Extract the HSV color space from image
// }
cv::Mat proMats[MAX_COLORS];
cv::Mat proMats_c[MAX_COLORS];

vector<CommObject*> objects;

int min_distance=53, param1=264, param2=23;
int minRadius=4, maxRadius=16;
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

	cv::blur(rawMat, rawMat, cv::Size(2,2));
	cvtColor(rawMat, hsvMat, COLOR_BGR2HSV);
	 
	for(int i = 0; i < MAX_COLORS; i++){
		proMats[i] = ip::processMat(hsvMat, our_colors[i]);
		proMats_c[i] = proMats[i].clone();
	}
	
	objects.clear();

	for(int i = 0; i < MAX_COLORS; i++){
		ip::find_cubes(proMats[i], squares);
		if (squares.size() > 0){
			CommPoly *o = new CommPoly();
			// Setting averages
			//   Finding center
			for(auto p : squares[0]){o->x += p.x; o->y += p.y;}
			o->x /= squares[0].size();
			o->y /= squares[0].size();
			//   Finding sizes
			float mx=0, my=0, minx=rawMat.cols, miny=rawMat.rows;
			for(auto p : squares[0]){
				if(p.x > mx)mx=p.x; 
				if(p.y > my)my=p.y; 
				if(p.x < minx)minx=p.x; 
				if(p.y < miny)miny=p.y;} 
			// for(auto p : squares[0]){o.s += sqrt( pow(o.x - p.x,2) + pow(o.y - p.y,2) );}
			o->sx = mx - minx;
			o->sy = my - miny;
			// o.s /= squares[0].size();
			// o.square = true;
			o->color = i;
			// Normalizing
			o->x /= rawMat.cols;
			o->y /= rawMat.rows;
			o->sx /= rawMat.cols;
			o->sy /= rawMat.rows;
			// Pushing
			objects.push_back(o);
		}
		#if defined(DISPLAY)
        ip::draw_cubes(finalMat, squares, ip::colors_bgr[i]);
		#endif
	}

	for(int i = 0; i < MAX_COLORS; i++){
		ip::color_num = i;
        ip::find_balls(proMats[i], (double)min_distance, (double)param1, (double)param2, minRadius, maxRadius);
		if(ip::circle_s_oa[i].size() > 0){
			for(auto c:ip::circle_s_oa[i]){
				CommCircle *o = new CommCircle();
				// Setting and normalizing
				o->x = c[0] / rawMat.cols;
				o->y = c[1] / rawMat.rows;
				o->r = c[2] / ((rawMat.cols + rawMat.rows) / 2);
				// o.square = false;
				o->color = i;
				// Pushing
				objects.push_back(o);
			}
		}
		#if defined(DISPLAY) && defined(DISPLAY_COLORS)
        ip::draw_balls(proMats_c[i], ip::colors_bgr[i]);
		#endif
	}
	
	#ifdef DISPLAY
	ip::draw_all_balls(finalMat);
	#endif
}


const char* window_names[] = {"FilterBlue", "FilterGreen", "FilterRed", "FilterYellow"};
void display(){
    // if(!rawMat.empty())imshow("Raw", rawMat);
	#if defined(DISPLAY)
	if(!finalMat.empty())imshow("Final", finalMat);
	#endif
	#if defined(DISPLAY) && defined(DISPLAY_COLORS)
	for(int i=0;i<MAX_COLORS;i++)imshow(window_names[i], proMats_c[i]);
	#endif
    // imshow("FilterBlue", proMats[0]);
	// imshow("FilterGreen", proMats[1]);
	// imshow("FilterRed", proMats[2]);
	// imshow("FilterYellow", proMats[3]);
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
    cap = startCamera(CAMERA_NUM);
#ifdef DISPLAY
    namedWindow("HoughCircles");
	createTrackbar("Min Distance", "HoughCircles", &min_distance, 100);
	createTrackbar("Param 1", "HoughCircles", &param1, 450);
	createTrackbar("Param 2", "HoughCircles", &param2, 450);
	createTrackbar("Min R", "HoughCircles", &minRadius, 400);
	createTrackbar("Max R", "HoughCircles", &maxRadius, 400);
	// createTrackbar("Min Saturation", "HoughCircles", &min_sat, 255);
	// createTrackbar("Min Value", "HoughCircles", &min_val, 255);
#endif
#if defined(DISPLAY) && defined(DISPLAY_COLORS)
	for(int i=0;i<MAX_COLORS;i++){
		namedWindow(window_names[i]);
		createTrackbar("Min S", window_names[i], &our_colors[i].minS, 255);
		createTrackbar("Min V", window_names[i], &our_colors[i].minV, 255);
	}
#endif
	
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

		ByteConstructor bc;
		CommObject::GetBytes(objects, bc);
		
		// if (bc.getSize()>0){
		// 	int s;ByteReceiver br(bc.getBytes(), bc.getSize());
		// 	CommObject ** ob = CommObject::GetObjects(br, s);
		// 	for (int i=0;i<s;i++)ob[i]->print();
		// }
		
		myComm.sendToNav(bc.getBytes(), bc.getSize());
		for(auto o:objects)delete o; // Clearing up all objects

        display();
    }
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	
	return 0;
}




