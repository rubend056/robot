#include "byte_helper.h"

// ByteConstructor **********
void ByteConstructor::update(int new_size){
    if (new_size > m){
        if(bytes){
            unsigned char * _bytes = new unsigned char[new_size];
            memcpy (_bytes, bytes, m);
            delete[] bytes;
            bytes = _bytes;
        }else {delete[] bytes; bytes = new unsigned char[new_size];}
        m = new_size;
    }
}

void ByteConstructor::add(const void *v, int size){
	update(c + size);
	memcpy(bytes+c, v, size);
	c+=size;
}