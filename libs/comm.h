#ifndef COMM_H
#define COMM_H

class Comm
{  
    public: 
        Comm(); 
        ~Comm();
        void sendToNav(char command);
        void receiveNav();
        int serialNav;


};

#endif