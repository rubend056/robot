#include "comm.h"
#include <wiringSerial.h>
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
		cout << r;
		serialPutchar(s,r);
	}
}

void Comm::sendToNav(uint32_t* floats, int n){
	if (serialNav < 0)return;
	cout << "There are " << n << " floats" << endl;
	int num_bytes = n*(sizeof(uint32_t));
	for(int i=0;i<n;i++)sendToNavBytes(floats, num_bytes, serialNav); // Sent all floats
	sendToNavBytes(((uint16_t*)&num_bytes), 2, serialNav); // Sent checksum
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