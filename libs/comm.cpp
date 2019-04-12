#include <wiringSerial.h>
#include "comm.h"
#include <iostream>


using namespace std;

Comm::Comm()
{
	serialNav = serialOpen("/dev/ttySAC0",115200);
}

void printBits(uint8_t* j){
	for (int i=7;i>=0;i--){
		cout << ((j[0] >> i) & 1) ? '1' : '0';
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