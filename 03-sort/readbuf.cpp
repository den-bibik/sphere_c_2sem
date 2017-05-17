#include "readbuf.hpp"
#include <iostream>
using namespace std;

readbuf::readbuf(string name, size_t bufsize_){
	//cout << "readbuf created" << endl;
	file = ifstream(name, std::ios::binary | std::ios::ate);
	bufsize = bufsize_;
	
	bytes_to_end = file.tellg();
	file.seekg(0);

	bufsize = bytes_to_end < bufsize ? bytes_to_end : bufsize;
	buf = new int[bufsize / sizeof(int)];
	for(int i = 0; i < bufsize / sizeof(int); i++) buf[i] = 0;
	
	//cout << bytes_to_end << endl;
	read_to_buf();
}

bool readbuf::read_to_buf(){
	//return false;
	
	if(file){
		size_t read = bytes_to_end < bufsize ? bytes_to_end : bufsize;
		cout << "r2b " << read  << "B" << endl;
		if(read <= 0) return false;
		if((bool) file.read(reinterpret_cast<char*>(buf), read)) {
			cout << "read_to_buf " << read  << "B" << endl;
			bytes_to_end -= read;
			point = buf;
			end = buf + read / sizeof(int);
			return true;
		}
		else return false;
	}
	else return false;
}

bool readbuf::read(int &to){
	//cout << "read enter" << endl;
	if(point >= end){
		if(!read_to_buf()) return false;
	}
	to = *point;
	point++;
	//cout << "readed" << endl;
	return true;
}
