#include "main.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <sstream>

string Object::getFileName(){ //We'll implement this later
	string name = "";
	switch(type){
		Cube:
			name += "cube";
			break;
		Sphere:
			name += "sphere";
			break;
	}
	name += "_" + to_string(x);
	name += "_" + to_string(y);
	name += "_" + to_string(w);
	name += "_" + to_string(h);
	return name;
}

int getInt(string string){
	return atoi(string.c_str());
}

void Object::useFilename(string name){
	auto dotIndex = name.find(".");
	if(dotIndex != -1){ //Deleting any extension still prevailing in the name
		name = name.substr(dotIndex, name.length() - dotIndex);
	}
	
	vector<string> strings;
	boost::split(strings, name, boost::is_any_of("_"));
	if(strings.size() < 5){cout << boost::format{"Size %1 less than 5"} % strings.size() << endl; return;} //Check if size less than 5
	
	int typeint = 0;
	
	if(strings[0] == "cube")typeint = 0;
	if(strings[0] == "sphere")typeint = 1;
	
	type = (Type)typeint; //Set type
	x = getInt(strings[1]); //Set posx
	y = getInt(strings[2]); //Set posy
	w = getInt(strings[3]); //Set width
	h = getInt(strings[4]); //Set height
}

vector<Object> Object::getObjects(string name){
	vector<string> objectStrings;
	boost::split(objectStrings, name, boost::is_any_of("|"));
	vector<Object> objects;
	for(auto it = objectStrings.begin(); it!=objectStrings.end(); ++it){
		objects.push_back(Object(*it));
	}
	return objects;
}