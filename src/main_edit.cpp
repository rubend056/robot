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
const int K_a = 97, K_s = 115, K_d = 100, K_w = 119;
const int K_f = 102, K_g = 103, K_h = 104, K_t = 116;
const int K_shift = 225, K_ctrl = 227;
const int K_R = 114;

Mat image;
Object o;
Browser *br;

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

	auto count_text = (boost::format("%1%/%2%") % br->file_count % br->availables.size()).str();
	putText(header, count_text, Point(2,height+4), font_face, font_size, font_color, font_width);
	
	string path_string = br->getPath().string();
	auto path_size = getTextSize(, font_face, font_size, font_width, &baseline);
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

void load_image(){
	if (br->is_valid() && !br->directory){
		image = imread(br->getPath());
	}else image.release();
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);

	if (!checkArg())return 1;
	auto rawPath = fs::path(useArg());
	if (!fs::exists(rawPath) || !fs::is_directory(rawPath)){cout << "The path must exist and it must be a directory" << endl; return 1;}
	
	Browser browser(rawPath);
	br = &browser;
	
	current_path = browser.getPath();
	update_image();
	
	int xy_move_speed = 1;
	int wh_move_speed = 1;

	while(1){
		auto key = waitKey(0);
		switch(key){
			case K_ESC:
				return 0;
				break;
			
			// Browser inupt
			case K_LEFT:
				browser.out();
				load_image();
				break;
			case K_RIGHT:
				browser.in();
				load_image();
				break;
			case K_UP:
				browser.prev();
				load_image();
				break;
			case K_DOWN:
				browser.next();
				load_image();
				break;
			
			// Editing output
			case K_a:
				o.x -= xy_move_speed;
				break;
			case K_d:
				o.x += xy_move_speed;
				break;
			case K_w:
				o.y -= xy_move_speed;
				break;
			case K_s:
				o.y += xy_move_speed;
				break;
			
			case K_f:
				o.w -= wh_move_speed;
				break;
			case K_h:
				o.w += wh_move_speed;
				break;
			case K_t:
				o.h += wh_move_speed;
				break;
			case K_g:
				o.h -= wh_move_speed;
				break;
			
			
			default:
				cout << "Pressed: " << key << endl;

		}
		
		update_image();
	}
}