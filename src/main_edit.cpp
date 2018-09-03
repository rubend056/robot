#include "main.h"
#include "img_process.h"
#include "arg_helper.h"
#include <iostream>


using namespace cv;

class Browser{
	public:
		Browser(fs::path r_path):root_path(r_path.c_str()){clean();update();}
		Browser(string r_path):root_path(r_path){clean();update();}
		
		bool directory = true;
		
		int file_count = 0;
		std::vector<string> names;
		std::vector<string> availables;
		
		void next(){
			file_count++;
			
		}
		void back(){
			
		}
		void in(){
			if (!directory)return;
			
		}
		void out(){
			if (!directory)return;
			
		}
		fs::path getPath(){
			return fs::path(get_current_path());
		}
	private:
		void clean(){
			int tocheck = root_path.length()-1;
			if (root_path[tocheck] == '/')
				root_path.erase(tocheck, 1);
		}
		string get_current_path(){
			string out = root_path;
			for(auto name : names){
				out += '/' + name;
			}
			if (directory && availables.size()>0)out += '/' + availables[file_count];
			return out;
		}
		string get_base_path(){
			
		}
		void update(){
			auto current_path = get_current_path();
			directory = fs::is_directory(fs::path(current_path.c_str()));
			availables.clear();
			if(directory){
				auto availables_path = listFolder(fs::path(current_path.c_str()));
				for(auto path : availables_path){
					availables.push_back(path.filename().string());
				}
			}
			file_count = 0;
		}
		
		void append(string name){
			names.push_back(name);
		}
		void strip(){
			names.erase(--names.end());
		}
		string root_path;
};

void usage(const char* comm){
	cout << "Usage: " << comm << "  <path_to_raw_data_folder>" << endl;
}

fs::path current_path;
bool is_image = false;

const int K_LEFT = 37, K_RIGHT = 39, K_UP = 38, K_DOWN = 40, K_MOD = 16, K_ESC = 27;
const int K_R = 82;

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
	
	auto main_list = listFolder(rawPath);
	auto main_c_path = main_list.begin();
	current_path = rawPath;
	
	update_image();

	while(1){
		switch(waitKey(0)){
			case K_ESC:
				return 0;
				break;
			case K_LEFT:
				
				break;
			case K_RIGHT:
				
				break;
			case K_UP:
				
				break;
			case K_DOWN:
				
				break;

		}
		update_image();
	}
}