#include "browser.h"

// Browser::Browser(fs::path r_path):root_path(r_path.c_str()){begin();}
// Browser::Browser(string r_path):root_path(r_path)

bool Browser::is_directory(){return directory;}
bool Browser::is_valid(){return availables.size()>0 && file_count < availables.size() && file_count >= 0;}

void Browser::reload(){
	auto base_path = get_base_path();
	availables.clear();
	
	auto availables_path = listFolder(fs::path(base_path.c_str()));
	for(auto path : availables_path){
		availables.push_back(path.filename().string());
	}
}
void Browser::next(){
	file_count++;
	if(file_count>=availables.size())file_count = 0;
	update();
}
void Browser::prev(){
	file_count--;
	if (file_count<0)file_count = availables.size()-1;
	if (availables.size() == 0)file_count = 0;
	update();
}
void Browser::in(){
	if (is_valid() && directory)
		append(availables[file_count]);
}
void Browser::out(){
	strip();
}
fs::path Browser::getPath(){
	return fs::path(get_current_path());
}
fs::path Browser::getPath_dir(){
	return fs::path(get_base_path());
}


void Browser::begin(){
	clean();
	update_root();
}
void Browser::clean(){
	int tocheck = root_path.length()-1;
	if (root_path[tocheck] == '/')
		root_path.erase(tocheck, 1);
}

string Browser::get_base_path(){
	string out = root_path;
	for(auto name : names){
		out += '/' + name;
	}
	return out;
}
string Browser::get_current_path(){
	string out = get_base_path();
	if (is_valid())out += '/' + availables[file_count];
	return out;
}


void Browser::update(){
	if(!is_valid()){directory = false;return;}
	directory = fs::is_directory(fs::path(get_current_path().c_str()));
}
void Browser::update_root(){
	reload();
	file_count = 0;
	update();
}




void Browser::append(string name){
	names.push_back(name);
	update_root();
}
void Browser::strip(){
	if (names.size() == 0)return;
	names.erase(--names.end());
	update_root();
}