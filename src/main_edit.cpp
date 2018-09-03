#include "main.h"
#include "img_process.h"
#include "arg_helper.h"
#include "browser.h"
#include <iostream>


using namespace cv;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_raw_data_folder>" << endl;
}

fs::path current_path;
bool is_image = false;

const int K_LEFT = 81, K_UP = 82, K_RIGHT = 83, K_DOWN = 84, K_ESC = 27;
const int K_R = 114;

Mat image;
Object o;

auto header_size = Size(600,30);

int it_index, it_max;

void update_image(){
	Mat to_show;

	if (!image.empty())header_size.width = image.cols;

	// Make header and add all info
	Mat header(header_size, CV_8UC1, Scalar(0,0,0));
	auto height = header_size.height/2;
	Scalar font_color(255,255,255);
	int baseline = 0;
	auto font_face = FONT_HERSHEY_SIMPLEX;
	double font_size = 0.5;
	double font_width = 1;

	auto count_text = (boost::format("%1%/%2%") % it_index % it_max).str();
	putText(header, count_text, Point(2,height+4), font_face, font_size, font_color, font_width);
	
	auto path_size = getTextSize(current_path.string(), font_face, font_size, font_width, &baseline);
	putText(header, current_path.string(), Point(header_size.width/2 - path_size.width/2,height), 
		font_face, font_size, font_color, font_width);

	auto obj_string = o.getFileName();
	auto obj_size = getTextSize(obj_string, font_face, font_size, font_width, &baseline);
	putText(header, obj_string, Point(header_size.width - obj_size.width,height), 
		font_face, font_size, font_color, font_width);

	// Clone image and write objects
	if (!image.empty()){
		Mat image_show = image.clone();
		writeObjects(image_show, o);
		vconcat(header, image, to_show);
	}else
		to_show = header;

	// Show image
	imshow("Editing", to_show);
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);

	if (!checkArg())return 1;
	auto rawPath = fs::path(useArg());
	if (!fs::exists(rawPath) || !fs::is_directory(rawPath)){cout << "The path must exist and it must be a directory" << endl; return 1;}
	
	Browser browser(rawPath);
	
	current_path = browser.getPath();
	update_image();

	while(1){
		auto key = waitKey(0);
		switch(key){
			case K_ESC:
				return 0;
				break;
			case K_LEFT:
				browser.out();
				break;
			case K_RIGHT:
				browser.in();
				break;
			case K_UP:
				browser.prev();
				break;
			case K_DOWN:
				browser.next();
				break;
			default:
				cout << "Pressed: " << key << endl;

		}
		current_path = browser.getPath();
		update_image();
	}
}