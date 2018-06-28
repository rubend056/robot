#ifndef LOGIC_PROCESS
#define LOGIC_PROCESS

#include "main.h"
#include "comands.h"

namespace lp{
	
	//A function to be called when all objects have been identified to be in a certain location, or just the objects at the moment that the camera sees
	void process(vector<Object> objects);
	
}

#endif