#ifndef COMM_H
#define COMM_H

#include <stdint.h>

class Comm
{  
    public: 
        Comm(); 
        ~Comm();
        void sendToNav(uint32_t* floats, int n);
        void sendToNav(char command);
        void receiveNav();
        int serialNav;


};

#endif