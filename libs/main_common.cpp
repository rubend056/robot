#include "main.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <sstream>

#include "input.h"

bool tCascade = false;

static volatile sig_atomic_t keepRunning = true;
void intHandler(int dummy) {
	keepRunning = false;
	if(tCascade)
		exit(1);
}
bool checkSig(){
	return (bool)keepRunning;
}

cv::Mat getHSV(cv::Mat mat){
	cv::Mat hsv;
	cv::cvtColor(mat, hsv, cv::COLOR_BGR2HSV);
	return hsv;
}

string Object::getFileName(){ //We'll implement this later
	string name = "";
	switch(type){
		case Cube:
			name += "cube";
			break;
		case Sphere:
			name += "sphere";
			break;
	}
	name += "_" + to_string(x);
	name += "_" + to_string(y);
	name += "_" + to_string(w);
	name += "_" + to_string(h);
	return name;
}

void Object::useFilename(string name){
	vector<string> strings;
	boost::split(strings, name, boost::is_any_of("_"));
	if(strings.size() < 5){/*cout << "Size " << strings.size() << " less than 5" << endl; */return;} //Check if size less than 5
	
	if(strings[0] == "cube")type = Cube;
	if(strings[0] == "sphere")type = Sphere;
	
	x = getInt(strings[1]); //Set posx
	y = getInt(strings[2]); //Set posy
	w = getInt(strings[3]); //Set width
	h = getInt(strings[4]); //Set height
}

vector<Object> Object::getObjects(string name){
	vector<string> objectStrings;
	
	boost::split(objectStrings, name, boost::is_any_of("+"));
	
	vector<Object> objects;
	for(auto it = objectStrings.begin(); it!=objectStrings.end(); ++it)
		objects.push_back(Object(*it));
	
	return objects;
}

string Object::getString(vector<Object> objects){
	string r = "";
	string l = "";
	for(auto it = objects.begin(); it != objects.end(); ++it){
		if(l != "")r+= "+";
		l = (*it).getFileName();
		r += l;
	}
	return r;
}

void writeObjects(cv::Mat src, vector<Object> objs){
	for(auto it = objs.begin(); it != objs.end(); ++it)
		cv::rectangle(src, cv::Rect2i(it->x, it->y, it->w, it->h), cv::Scalar(0), 2);
}
void writeObjects(cv::Mat src, Object obj){
	vector<Object> objs = {obj};
	writeObjects(src, objs);
}



void clean(std::string &data_path){
	int tocheck = data_path.length()-1;
	if (data_path[tocheck] == '/')
		data_path.erase(tocheck, 1);
}

std::string get_count_string(int count){
	return (boost::format("%|06d|") % count).str();
}

void save_DB_image(std::string folder, int count, Object &o, cv::Mat &mat){
	clean(folder);
	
	cv::imwrite(folder + "/" + get_count_string(count) + "-" + o.getFileName() + ".jpg", mat);
}
void load_DB_image(std::string path, int &count, Object &o, cv::Mat &mat){
	clean(path);
	
	mat = cv::imread(path);
	
	//Isolating filename
	int last_slash = path.find_last_of('/');
	if (last_slash == -1)last_slash = 0;
	auto filename = path.substr(last_slash + 1);
	
	//Setting up extension indexes
	auto dot_index = filename.find_first_of('.');
	auto min_index = filename.find_first_of('-');
	min_index++; // If not found, then 0, if found then on top of char
	if (dot_index == -1)dot_index = filename.size()-1;
	
	//Do the cutting
	auto obj_string = filename.substr(min_index, dot_index - min_index);
	
	if (min_index == 0)min_index = dot_index;else min_index--;
	auto counter_string = filename.substr(0, min_index);
	
	// cout << "Object string: " << obj_string << "   Counter: " << counter_string << endl;
	
	o.useFilename(obj_string);
	count = getInt(counter_string);
}

void set_and_print(cv::VideoCapture &cap, int propID, std::string propString, double value){
	cap.set(propID, value);
	cout << "Camera " << propString << ": " << cap.get(propID) << endl;
}

double hue = 0.5, sat = 0.5, cont = 0.5;
cv::VideoCapture startCamera(int index){
	auto cap = cv::VideoCapture(index);
	set_and_print(cap, cv::CAP_PROP_FPS, "FPS", 30);
	set_and_print(cap, cv::CAP_PROP_HUE, "Hue", hue);
	set_and_print(cap, cv::CAP_PROP_SATURATION, "Saturation", sat);
	set_and_print(cap, cv::CAP_PROP_CONTRAST, "Contrast", cont);
	
	// cout << "Camera " << cap.get(cv::CAP_PROP_GAMMA) << endl;
	// cout << "Camera " << cap.get(cv::CAP_PROP_AUTO_EXPOSURE) << endl;
	// cout << "Camera " << cap.get(cv::CAP_PROP_EXPOSURE) << endl;
	// cout << "Camera " << cap.get(cv::CAP_PROP_EXPOSUREPROGRAM) << endl;
	// cout << "Camera " << cap.get(cv::CAP_PROP_BRIGHTNESS) << endl;
	// cout << "Camera " << cap.get(cv::CAP_PROP_TEMPERATURE) << endl;
	
	// Check if camera opened successfully
	if(!cap.isOpened()){
		cout << "Error opening video stream" << endl;
		exit(1);
	}
	return cap;
}
bool handleCameraInput(cv::VideoCapture &cap, int key){
	switch(key){
		case K_z:
			hue -= 0.01;
			set_and_print(cap, cv::CAP_PROP_HUE, "Hue", hue);
			break;
		case K_x:
			hue += 0.01;
			set_and_print(cap, cv::CAP_PROP_HUE, "Hue", hue);
			break;
		case K_c:
			sat -= 0.01;
			set_and_print(cap, cv::CAP_PROP_SATURATION, "Saturation", sat);
			break;
		case K_v:
			sat += 0.01;
			set_and_print(cap, cv::CAP_PROP_SATURATION, "Saturation", sat);
			break;
		case K_b:
			cont -= 0.01;
			set_and_print(cap, cv::CAP_PROP_CONTRAST, "Contrast", cont);
			break;
		case K_n:
			cont += 0.01;
			set_and_print(cap, cv::CAP_PROP_CONTRAST, "Contrast", cont);
			break;
		default:
			return false;
	}
	return true;
}