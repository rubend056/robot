#include "main.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <sstream>

void intHandler(int dummy) {
    keepRunning = 0;
}

bool generalFolder(string name){ // General folders hold no object data in folder name and all files provide their own data
	return name.find("_") == -1;
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
	//Deleting any extension still prevailing in the name
	auto dotIndex = name.find(".");
	if(dotIndex != -1){ 
		name.substr(dotIndex, name.length() - dotIndex);
	}
	//Deleting any "other" extension still prevailing in the name
	dotIndex = name.find("-");
	if(dotIndex != -1){ 
		name.substr(dotIndex, name.length() - dotIndex);
	}
	
	vector<string> strings;
	boost::split(strings, name, boost::is_any_of("_"));
	if(strings.size() < 5){cout << "Size " << strings.size() << " less than 5" << endl; return;} //Check if size less than 5
	
	if(strings[0] == "cube")type = Cube;
	if(strings[0] == "sphere")type = Sphere;
	
	x = getInt(strings[1]); //Set posx
	y = getInt(strings[2]); //Set posy
	w = getInt(strings[3]); //Set width
	h = getInt(strings[4]); //Set height
	
}

vector<Object> Object::getObjects(string name){
	vector<string> objectStrings;
	// cout << "Splitting " << name << endl;
	boost::split(objectStrings, name, boost::is_any_of("|"));
	// cout << "Done splitting " << endl;
	vector<Object> objects;
	for(auto it = objectStrings.begin(); it!=objectStrings.end(); ++it){
		objects.push_back(Object(*it));
	}
	return objects;
}

string Object::getString(vector<Object> objects){
	string r = "";
	string l = "";
	for(auto it = objects.begin(); it != objects.end(); ++it){
		if(l != "")r+= "|";
		l = (*it).getFileName();
		r += l;
	}
	return r;
}

cv::Mat writeObjects(cv::Mat src, vector<Object> objs){
	cv::Mat out;
	src.copyTo(out);
	
	const int textBackW = 100, textBackH = 40;
	
	for(auto it = objs.begin(); it != objs.end(); ++it){
		cv::rectangle(out, cv::Rect2i(it->x, it->y, it->w, it->h), cv::Scalar(0), 2);
		/*
		cv::rectangle(out, cv::Rect2i(
									it->x - it->w + textBackW / 2, 
									it->y - it->h - textBackH / 2, 
									textBackW, textBackH), 
									cv::Scalar(0,0,0), CV_FILLED);
		*/
	}
	
	return out;
}
cv::Mat writeObjects(cv::Mat src, Object obj){
	vector<Object> objs = {obj};
	return writeObjects(src, objs);
}