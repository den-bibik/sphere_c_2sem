#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>
#include <sstream> 
#include <string>
#include <stdio.h>
#include "merge.hpp"


using namespace std;
int main(int argc, char** argv){
	const char * INPUT_FILE = argv[1];
	const char * OUTPUT_FILE = argv[2];

	std::ifstream f1(INPUT_FILE,std::ifstream::ate | std::ios::binary);


	size_t buf_len = (size_t) atoll(argv[3]) * 1024 * 1024; 
	int k = atoi(argv[4]);
	size_t len = f1.tellg(); 
	cout << "Input file size " << len / 1024 / 1024 << "MB" <<endl;
	size_t position = 0;
	int num = 0; 
	
	int i = 0;
	while(len > position){
	
		cout << "sorting part " << num++ << endl;
		size_t l = buf_len;
		if(buf_len > len - position)
			l = len - position;

		std::vector<int> buf(l / sizeof(int));
		f1.read(reinterpret_cast<char*>(buf.data()), buf.size()*sizeof(int)); 
		int j = 0;
		sort(buf.begin(), buf.end());

		stringstream  adr;
		adr << "tmp/0." << i;
		std::ofstream tmp(adr.str(),std::ios::binary);
		tmp.write(reinterpret_cast<const char*>(&buf[0]), buf.size()*sizeof(int));
		position += buf_len;
		i++;
	}
	int l = 0;
	while(i > 1){
		for(int j = 0; j < i; j += k){
			vector<string> names;	
			cout << "merging parts: ";
			for(int f_num = j; (f_num < j + k) && (f_num < i); f_num++)
			{
				stringstream  adr;
				adr << "tmp/"<<l<<"." << f_num;
				names.push_back(adr.str());
				cout << adr.str() << "  ";
			}
			
			string nm;
			if(i > k){
				stringstream  out;
				out << "tmp/"<<l + 1<<"." << j / k;
				nm = out.str();
			}
			else nm = OUTPUT_FILE;
			
			cout << " to " << nm << endl;
			merge(names, nm);
			for(string name: names) remove(name.c_str());
		}
		l++;
		i = i / k + (i % k > 0);
	}

	return 0;

}

