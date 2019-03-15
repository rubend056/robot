#include "comm.h"
#include <wiringSerial.h>
#include "string"
#include <unistd.h>
#include <iostream>

using namespace std;

Comm::Comm()
{
	serialNav= serialOpen("/dev/ttySAC0",115200);
	
}
 
void Comm::sendToNav(char command)
{
	serialPutchar(serialNav,command);
	serialFlush(serialNav);
}

void Comm::receiveNav()
{
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