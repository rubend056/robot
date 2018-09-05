#include "main.h"
#include "img_process.h"
#include "arg_helper.h"
#include "browser.h"
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace cv;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_raw_data_folder>" << endl;
}

const int K_LEFT = 81, K_UP = 82, K_RIGHT = 83, K_DOWN = 84, K_ESC = 27;
const int K_a = 97, K_s = 115, K_d = 100, K_w = 119;
const int K_f = 102, K_g = 103, K_h = 104, K_t = 116;
const int K_shift = 225, K_ctrl = 227;
const int K_r = 114;

Mat image;
Object o;
Browser *br;

auto header_size = Size(1000,30);

int xy_move_speed = 4;
// int wh_move_speed = 1;

void update_image(){
	Mat to_show;

	if (!image.empty())header_size.width = image.cols;

	// Make header and add all info
	Mat header(header_size, CV_8UC3, Scalar(0,0,0));
	auto height = header_size.height/2;
	Scalar font_color(255,255,255);
	int baseline = 0;
	auto font_face = FONT_HERSHEY_SIMPLEX;
	double font_size = 0.5;
	double font_width = 1;

	auto count_text = (boost::format("%1%/%2%") % br->file_count % br->availables.size()).str();
	putText(header, count_text, Point(2,height+4), font_face, font_size, font_color, font_width);
	
	string path_string = br->getPath().string();
	auto path_size = getTextSize(path_string, font_face, font_size, font_width, &baseline);
	putText(header, path_string, Point(header_size.width/2 - path_size.width/2,height), 
		font_face, font_size, font_color, font_width);

	auto obj_string = to_string(xy_move_speed) + "  " +  o.getFileName();
	auto obj_size = getTextSize(obj_string, font_face, font_size, font_width, &baseline);
	putText(header, obj_string, Point(header_size.width - obj_size.width,height), 
		font_face, font_size, font_color, font_width);

	// Clone image and write objects
	if (!image.empty()){
		Mat image_show = image.clone();
		writeObjects(image_show, o);
		vconcat(header, image_show, to_show);
	}else
		to_show = header;

	// Show image
	imshow("Editing", to_show);
}

bool editing = false;
fs::path edit_path;
int image_count = 0;

void reset_object(){
	o.x = 300;
	o.y = 300;
	o.w = 200;
	o.h = 200;
}

void save_image(){
	if (editing){
		// cout << "Image has cols " << image.cols << endl;
		fs::remove(edit_path);
		imwrite(br->getPath_dir().string() + '/' + to_string(image_count) + '-' + o.getFileName() + ".jpg", image);
		image.release();
		br->reload();
		editing = false;
	}
}

void load_image(){
	if (editing)
		save_image();
	
	if (br->is_valid() && !br->directory){
		// Read the image
		edit_path = br->getPath();
		image = cv::imread(edit_path.string());
		
		// Decompose the filename
		vector<string> segments;
		boost::split(segments, br->getPath().filename().string(), boost::is_any_of("-"));
		
		image_count = getInt(segments[0]);
		if (segments.size() > 1)
			o.useFilename(segments[1]);
		else {
			reset_object();
			o.x = image.cols/2 - o.w/2;
			o.y = image.rows/2 - o.h/2;
		}
		
		editing = true;
	}else {image.release();editing = false;}
}

int main(int argc, char** argv)
{
	setArgs(argc, argv);

	if (!checkArg())return 1;
	auto rawPath = fs::path(useArg());
	if (!fs::exists(rawPath) || !fs::is_directory(rawPath)){cout << "The path must exist and it must be a directory" << endl; return 1;}
	
	Browser browser(rawPath);
	br = &browser;
	
	load_image();
	update_image();
	
	

	while(1){
		auto key = waitKey(0);
		switch(key){
			case K_ESC:
				return 0;
				break;
			
			// Browser input
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
				o.w -= xy_move_speed;
				break;
			case K_h:
				o.w += xy_move_speed;
				break;
			case K_t:
				o.h -= xy_move_speed;
				break;
			case K_g:
				o.h += xy_move_speed;
				break;
				
			case K_r:
				o.x = 0;
				o.y = 0;
				o.w = 20;
				o.h = 20;
				break;
			
			case K_shift:
				xy_move_speed++;
				break;
			case K_ctrl:
				xy_move_speed--;
				break;
			
			
			default:
				cout << "Pressed: " << key << endl;

		}
		
		update_image();
	}
}