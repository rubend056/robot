#ifndef COMM_H
#define COMM_H

#include "main.h"
#include <stdint.h>

class Comm
{  
    public: 
        Comm(); 
        ~Comm();
        void sendToNav(uint32_t* floats, int n);
        void sendToNav(uint8_t* d, int n);
        void sendToNav(char command);
        void receiveNav();
        int serialNav;


};

class CommObject{
    public:
        float x=0, y=0, s=0;
        bool square = false;
        uint8_t color = 0;
        // uint8_t type=0; // If 0 bit is on, then square else sphere, 1-7th bits are used for colors, since only 4 colors only 6-7 bits will be used\
        meaning blue, green, red, yellow, in that order.
    	static CommObject* getObjects(uint8_t* d, int n, int* out_n);
		static uint8_t* getBytes(std::vector<CommObject> objects, int* n);
};

#endif