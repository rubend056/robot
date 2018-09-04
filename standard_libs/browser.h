#ifndef BROWSER
#define BROWSER

#include "misc.h"

namespace fs = boost::filesystem;
using namespace std;

class Browser{
	public:
		Browser(fs::path r_path):root_path(r_path.c_str()){begin();};
		Browser(string r_path):root_path(r_path){begin();};
		
		int file_count = 0;
		std::vector<string> availables;
		
		bool is_directory();
		bool is_valid();
		
		void next();
		void prev();
		void in();
		void out();
		fs::path getPath();
	private:
		void begin();
		string root_path;
		
		bool directory = true;
		
		
		
		std::vector<string> names;
		
		
		void clean();
		
		string get_base_path();
		string get_current_path();
		void update();
		void update_root();
		
		void append(string name);
		void strip();
};

#endif