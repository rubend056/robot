#ifndef COMM_H
#define COMM_H

#include "misc.h"
#include <vector>
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
        
        float x=0, y=0;
        uint8_t color = 0;
        bool poly = false;
        CommObject(){}
        CommObject(const CommObject &o):x(o.x),y(o.y),color(o.color),poly(o.poly){}
        virtual void serialize(ByteConstructor &bc);
        
        virtual void print(){
            #ifdef ARDUINO
            Serial.print(String(x) + " " + String(x) + " " + String(color) + " " + (poly) ? "poly": "circle");
            #else
            cout << x << " " << y << " " << (int)color << " " << ((poly)?"poly":"circle" );
            #endif
        }
        
        virtual void deserialize(ByteReceiver &br);
        static CommObject* Deserialize(ByteReceiver &br);
        
        // uint8_t type=0; // If 0 bit is on, then square else sphere, 1-7th bits are used for colors, since only 4 colors only 6-7 bits will be used\
        meaning blue, green, red, yellow, in that order.
        static void GetBytes(std::vector<CommObject*> &objects, ByteConstructor& bc);
    	static CommObject** GetObjects(ByteReceiver &br, int& s);
        static void DeleteObjects(CommObject** objects, int s);
};


class CommPoly :public CommObject{
public:
    float sx=0, sy=0;
    CommPoly(){poly=true;}
    CommPoly(const CommObject &o):CommObject::CommObject(o){}
    void serialize(ByteConstructor &bc);
    void deserialize(ByteReceiver &br);
    void print(){
        CommObject::print();
        #ifdef ARDUINO
        Serial.println(" " + String(sx) + " " + String(sy));
        #else
        cout << " " << sx << " " << sy << endl;
        #endif
    }
};

class CommCircle:public CommObject{
public:
    float r=0;
    CommCircle(){poly=false;}
    CommCircle(const CommObject &o):CommObject::CommObject(o){}
    void serialize(ByteConstructor &bc);
    void deserialize(ByteReceiver &br);
    void print(){
        CommObject::print();
        #ifdef ARDUINO
        Serial.println(" " + String(r));
        #else
        cout << " " << r << endl;
        #endif
    }
};

#endif