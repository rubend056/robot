#include "main.h"
#include "arg_helper.h"

#include <time.h>

using namespace cv;

VideoCapture cap;
Mat rawMat, showMat;
int image_count = 0;

string data_path;

const int K_ESC = 27, K_r = 114, K_space = 32;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_data_folder> <arguments>" << endl;
	cout << "	-cam   <int camera_numb = 0>" << endl;
}

void startCap(int camera_numb = 0){
	cap = cv::VideoCapture(camera_numb);
	
	// Check if camera opened successfully
	if(!cap.isOpened()){
		cout << "Error opening video stream" << endl;
		exit(1);
	}
}

void capture(){
	cap >> rawMat;
}

bool recording = false;
bool save_picture = false;
clock_t record_timer;
int circle_time = 500; // About one second circle time
clock_t record_timer_circle;
void overlay(){
	showMat = rawMat.clone();
	
	auto now = clock();
	
	auto height = 10;
	Scalar font_color(255,255,255);
	int baseline = 0;
	double font_size = 0.4;
	double font_width = 1;
	
	auto count_text = to_string(image_count);
	cv::putText(showMat, count_text, Point(2,height+4), font_face, font_size, font_color, font_width);
	
	if (recording){
		cv::circle(showMat, cv::Point(showMat.cols - 10, height), 5, cv::Scalar(0,0,255),3);
	}
	
}

void display(){
	cv::imshow("Camera", showMat);
}

void save(){
	if (recording || save_picture){
		std::string count_string = (boost::format("%|06d|") % image_count).str();
		auto save_path = concatPath(data_path, count_string + ".jpg");
		cv::imwrite(save_path.string(), rawMat);
		image_count++;
		save_picture = false;
	}
}


void start_video(){
	record_timer = record_timer_circle = clock(); // Zeroing out the clocks
	recording = true;
}
void stop_video(){
	recording = false;
}
void take_picture(){
	save_picture = true;
}


int main(int argc, char** argv)
{
	setArgs(argc, argv);
	
	c_arg = 1;
	if(!checkArg())return 1;
	
	// Getting data path root
	data_path = getArg();
	
	// Cleaning the data path root
	int tocheck = data_path.length()-1;
	if (data_path[tocheck] == '/')
		data_path.erase(tocheck, 1);
	
	int camera_numb = 0;
	while(checkArg(0, false)){
		string c(useArg());
		if(c == "-cam"){
			unsigned int var = atoi(useArg().c_str());
			camera_numb = var;
			cout << "Camera index set to " << var << endl;
		}
	}
	
	startCap(camera_numb);
	
	// Get every key pressed and store it in a vector, while waiting for the right time and update
	bool running = true;
	while(running){
		clock_t start = clock();
		clock_t delta = 0;
		std::vector<int> pressed;
		do{
			auto key = waitKey(1);
			if(key >= 0)
				pressed.push_back(key);
			delta = clock() - start;
		}while(delta < 30);
		
		for(auto key : pressed){
			switch(key){
				case K_ESC:
					running = false;
					break;
				case K_space:
					take_picture();
					break;
				case K_r:
					if (recording)
						stop_video();
					else
						start_video();
					break;
				default:
					std::cout << "Pressed: " << key << std::endl;
					break;
			}
		}
		capture();
		save();
		overlay();
		display();
	}
	
	// std::ofstream countf(data_path + "/count.txt");
	// if(countf.is_open()){
	// 	std::string line;
	// 	getline(countf, line);
	// 	import_count = getInt(line);
	// 	countf.close();
	// }
	
	if(cap.isOpened())cap.release();
	destroyAllWindows();
	
	return 0;
}




