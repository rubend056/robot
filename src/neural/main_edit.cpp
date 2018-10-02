#include "main.h"
#include "img_process.h"
#include "arg_helper.h"
#include "browser.h"
#include "input.h"
#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace cv;

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_raw_data_folder>" << endl;
}

Mat image;
Object o;
Browser *br;

bool changed = false;
auto header_size = Size(500,20);

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
	double font_size = 0.4;
	double font_width = 1;

	auto count_text = (boost::format("%1%/%2%") % br->file_count % br->availables.size()).str();
	auto count_color = font_color;
	if (changed){count_text += "  Changed";count_color = cv::Scalar(0,255,255);}
	putText(header, count_text, Point(2,height), font_face, font_size, count_color, font_width);
	
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
	if (!image.empty()){
		o.x = image.cols/2 - o.w/2;
		o.y = image.rows/2 - o.h/2;
	}
}
void zero_object(){
	o.x = 0;
	o.y = 0;
	o.w = 0;
	o.h = 0;
}

void save_image(){
	if (editing && changed){
		fs::remove(edit_path);
		save_DB_image(br->getPath_dir().string(), image_count, o, image);
		image.release();
		br->reload();
	}
	changed = false;
	editing = false;
}

void load_image(){
	save_image();
	
	if (br->is_valid() && !br->directory){
		edit_path = br->getPath();
		load_DB_image(edit_path.string(), image_count, o, image);
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
			case K_y:
				browser.next();
				load_image();
				break;
			case K_u:
				browser.prev();
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
				reset_object();
				break;
			case K_q:
				zero_object();
				changed = true;
				break;
			
			
			case K_shift:
				xy_move_speed++;
				break;
			case K_ctrl:
				xy_move_speed--;
				break;
			
			case K_e:
				changed = !changed;
				break;
			
			default:
				cout << "Pressed: " << key << endl;

		}
		switch(key){
			case K_a:
			case K_d:
			case K_w:
			case K_s:
			case K_f:
			case K_h:
			case K_t:
			case K_g:
				changed = true;
				break;
		}
		
		update_image();
	}
}