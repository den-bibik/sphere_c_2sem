#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <algorithm>


using namespace std;
int main(int argc, char** argv){
	const char * INPUT_FILE = argv[1];
	const char * OUTPUT_FILE = argv[2];

	std::ifstream f1(INPUT_FILE,std::ifstream::ate | std::ios::binary);
	std::ofstream fo(OUTPUT_FILE,std::ios::binary);
	std::ifstream sf(OUTPUT_FILE,std::ios::binary);

	size_t buf_len = (size_t) atoll(argv[3]) * 1024 * 1024; 
	size_t len = f1.tellg(); 
	cout << "Input file size " << len / 1024 / 1024 << "MB" <<endl;
	size_t position = 0;
	int num = 0; 

	while(len > position){

		cout << "sorting part " << num++ << endl;
		f1.seekg(position);
		size_t l = buf_len;
		if(buf_len > len - position)
			l = len - position;

		std::vector<int> buf(l / sizeof(int));
		f1.read(reinterpret_cast<char*>(buf.data()), buf.size()*sizeof(int)); 
		int j = 0;
		sort(buf.begin(), buf.end());
		//fo.seekg(position);
		fo.write(reinterpret_cast<const char*>(&buf[0]), buf.size()*sizeof(int));
		position += buf_len;
	}
	
	buf_len /= 2;
	int parts = len / buf_len + (int)(len % buf_len > 0);
	for(int i = 0; i < parts; i++){
		for(int j = i + 1; j < parts; j++){
			cout << "merge parts " << i << ", " << j << endl;
			//read i and j part
			position = i * buf_len;
		
			size_t l1 = buf_len, l2 = buf_len;
			if(buf_len > len - i * buf_len)
				l1 = len - i * buf_len;
			if(buf_len > len - j * buf_len)
				l2 = len - j * buf_len;
			std::vector<int>  buf((l1 + l2) / sizeof(int));	

			position = i * buf_len;
			sf.seekg(position);
			sf.read(reinterpret_cast<char*>(buf.data()), l1); 

			position = j * buf_len;
			sf.seekg(position);
			sf.read(reinterpret_cast<char*>(buf.data()) + l1, l2); 
			//merge
			std::inplace_merge(buf.begin(), buf.begin() + l1 / sizeof(int), buf.end());
			//write		
			position = i * buf_len;
			fo.seekp(position);
			fo.write(reinterpret_cast<const char*>(buf.data()), l1);

			position = j * buf_len;
			fo.seekp(position);
			fo.write(reinterpret_cast<const char*>(buf.data()) + l1, l2);
		} 	
	}

	return 0;

}

