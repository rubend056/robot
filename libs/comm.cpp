#include <wiringSerial.h>
#include "comm.h"
#include "string"
#include <unistd.h>
#include <iostream>
#include <vector>


using namespace std;

Comm::Comm()
{
	serialNav = serialOpen("/dev/ttySAC0",115200);
}

void printBits(uint8_t* j){
	for (int i=0;i<8;i++){
		bool b = j[0] << i;
		cout << b ? '1' : '0';
	}
}
void sendToNavBytes(const void* p, int n, int s){
	for (int i=0;i<n;i++){
		unsigned char r = ((uint8_t*)p)[i];
		auto t = (int)r;
		cout << std::hex << t << std::dec;
		serialPutchar(s,r);
	}
}

void Comm::sendToNav(uint32_t* floats, int n){
	if (serialNav < 0)return;
	cout << "There are " << n << " floats" << endl;

	uint16_t num_bytes = n*(sizeof(uint32_t));
	sendToNavBytes(floats, num_bytes, serialNav); // Sent all floats
	sendToNavBytes(&num_bytes, 2, serialNav); // Sent checksum
	sendToNavBytes("\ne",2, serialNav); // Send end of file thing
	serialFlush(serialNav);
	
	cout << endl;
}
void Comm::sendToNav(uint8_t* d, int n){
	if (d == NULL || n == 0)return;
	cout << "There are " << n << " bytes" << endl;
	if (serialNav < 0)return;
	
	uint16_t num_bytes = n;
	sendToNavBytes(d, n, serialNav);
	sendToNavBytes(&num_bytes, 2, serialNav); // Sent checksum
	sendToNavBytes("\ne",2, serialNav); // Send end of file thing
	serialFlush(serialNav);
	
	cout << endl;
}
void Comm::sendToNav(char command)
{
	if (serialNav < 0)return;
	serialPutchar(serialNav,command);
	serialFlush(serialNav);
}

void Comm::receiveNav()
{
	if (serialNav < 0)return;
	char arSignal = serialGetchar(serialNav);
	//serialPutchar(serialNav,serialGetchar(serialNav));
	cout << arSignal << endl;
	serialFlush(serialNav);	
}
Comm::~Comm()
{
	cout << "The comm destructor called" << endl;
	serialClose(serialNav);
}

CommObject* CommObject::getObjects(uint8_t* d, int n, int* out_n){
	int osize = (4*3+1); *out_n = n / osize;
	CommObject *o = new CommObject[*out_n];
	for(int i=0;i<*out_n;i++){
		uint8_t* j = d + osize*i;
		o[i].x = *((float*)(j));
		o[i].y = *((float*)(j+4));
		o[i].s = *((float*)(j+8));
		o[i].square = (*(j+12) & (1 << 7)) ? 1 : 0;
		o[i].color = *(j+12) & 3;
	}
	return o;
}
uint8_t* CommObject::getBytes(vector<CommObject> objects, int* n){
	if (objects.size() == 0)return NULL;
	int osize = (4*3+1); *n = objects.size() * osize;
	uint8_t *d = (uint8_t*)malloc(*n); // We have our memory
	for(int i=0;i<objects.size();i++){
		uint8_t* j = d + osize*i;
		memcpy(j, &objects[i].x, 4);
		memcpy(j+4, &objects[i].y, 4);
		memcpy(j+8, &objects[i].s, 4);
		uint8_t t = ((objects[i].square) << 7) & (objects[i].color);
		memcpy(j+12, &t, 1);
	}
	return d;
}