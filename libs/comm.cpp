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


void CommObject::serialize(ByteConstructor &bc){
	bc.add(poly);
	bc.add(x);
	bc.add(y);
	bc.add(color);
}
void CommObject::deserialize(ByteReceiver &br){
	br.get(poly);
	br.get(x);
	br.get(y);
	br.get(color);
}
void CommObject::GetBytes(std::vector<CommObject*> &objects, ByteConstructor& bc){
	if (objects.size()==0)return;
	bc.add<uint8_t>(objects.size());
	for (auto o: objects)o->serialize(bc);
}
CommObject** CommObject::GetObjects(ByteReceiver &br, int& s){
	s = br.get<uint8_t>();
	CommObject **objects = new CommObject*[s];
	for(int i=0;i<s;i++){
		objects[i] = CommObject::Deserialize(br);
	}
	return objects;
}
void CommObject::DeleteObjects(CommObject** objects, int s){for(int i=0;i<s;i++)delete objects[i]; delete[] objects;}

CommObject* CommObject::Deserialize(ByteReceiver &br){
	CommObject o;
	o.deserialize(br);
	CommObject* p; if (o.poly) p = new CommPoly(o); else p = new CommCircle(o);
	p->deserialize(br);
	return p;
}

void CommPoly::serialize(ByteConstructor &bc){
	CommObject::serialize(bc);
	bc.add(sx);
	bc.add(sy);
}
void CommPoly::deserialize(ByteReceiver &br){
	br.get(sx);
	br.get(sy);
}

void CommCircle::serialize(ByteConstructor &bc){
	CommObject::serialize(bc);
	bc.add(r);
}
void CommCircle::deserialize(ByteReceiver &br){
	br.get(r);
}

// CommObject* CommObject::getObjects(uint8_t* d, int n, int* out_n){
// 	int osize = (4*3+1); *out_n = n / osize;
// 	CommObject *o = new CommObject[*out_n];
// 	for(int i=0;i<*out_n;i++){
// 		uint8_t* j = d + osize*i;
// 		o[i].x = *((float*)(j));
// 		o[i].y = *((float*)(j+4));
// 		o[i].s = *((float*)(j+8));
// 		o[i].square = (*(j+12) & (1 << 7)) ? 1 : 0;
// 		o[i].color = *(j+12) & 3;
// 	}
// 	return o;
// }
// uint8_t* CommObject::getBytes(vector<CommObject> objects, int* n){
// 	if (objects.size() == 0)return NULL;
// 	int osize = (4*3+1); *n = objects.size() * osize;
// 	uint8_t *d = (uint8_t*)malloc(*n); // We have our memory
// 	for(int i=0;i<objects.size();i++){
// 		// cout << i << " " << objects[i].x << " " << objects[i].y << " " << objects[i].s << " " << (objects[i].square ? '1' : '0') << " " << (int)objects[i].color << endl;
// 		uint8_t* j = d + osize*i;
// 		memcpy(j, &objects[i].x, 4);
// 		memcpy(j+4, &objects[i].y, 4);
// 		memcpy(j+8, &objects[i].s, 4);
// 		uint8_t t = ((objects[i].square) << 7) | (objects[i].color);
// 		// printBits(&t);
// 		memcpy(j+12, &t, 1);
// 	}
// 	return d;
// }