#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <fstream>
#include <algorithm>
#include <climits>
#include "readbuf.hpp"

using namespace std;



void merge(std::vector<std::string> file_names, std::string output){
	size_t buf_size = 750 * 1024 * 1024 / file_names.size();
	int read_count = 0, read_count_prev = 0;
	vector<readbuf> files;
	list<int> readed;
	for(string name:file_names) 
		files.push_back(readbuf(name, buf_size));
	ofstream out(output,std::ios::binary);

	vector<int> last(files.size(), INT_MIN);
	vector<int> toAdd;

	for(int main_file_num = 0; main_file_num < files.size(); main_file_num++){	
		int main_value = 0;
		while(files[main_file_num].read(main_value)){
			//cout << "while enter" << endl;
			/*if(read_count - read_count_prev > 10)
			{
				read_count_prev = read_count;
				cout << read_count  << " readed  " << readed.size() << "  " << toAdd.size() << endl;
			}*/

			last[main_file_num] = main_value;
			readed.push_back(main_value);
			for(int file_num = main_file_num + 1; file_num < files.size(); file_num++){
				int value = 0;
				
				while(last[file_num] <= main_value && files[file_num].read(value)){
					//cout << last[file_num] << "  "  << (last[file_num] <= main_value) << endl;
					readed.push_back(value);
					last[file_num] = value;
					read_count++;
					if(value > main_value) break;
				}
			}
			
			//cout << "before "<< readed.size() << endl;
			readed.remove_if([&toAdd, main_value](auto& i) {
				bool ok = (i <= main_value);
				if(ok) toAdd.push_back(i);
				return ok; 
			});
			//cout << "after "<< readed.size() << endl;
			sort(toAdd.begin(), toAdd.end());
			out.write(reinterpret_cast<const char*>(toAdd.data()), toAdd.size() * sizeof(int));
			
			
			toAdd.clear();
			//break;
			int main_value = 0;

		}
	}
	//cout << read_count << "res  " << readed.size() << endl;
	vector<int> v{ readed.begin(), readed.end()};

	sort(v.begin(), v.end());
	out.write(reinterpret_cast<const char*>(v.data()), v.size() * sizeof(int));

}