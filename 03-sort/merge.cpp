#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>

using namespace std;

void merge(vector<string> file_names, string output){
	vector<std::ifstream> files; 
	for(string name: file_names)
		files.push_back(std::ifstream(name, std::ios::binary));


	std::ofstream o(output,std::ios::binary);
	

	vector<bool> toRead(files.size(), true);
	vector<bool> upd(files.size(), false);
	vector<int> add;
	vector<int> vals(files.size());
	long long count = 0;
	size_t prev_t;
	bool read_i = true;
	
	int c_prev = 0;

	for(int i = 0; i < files.size(); i++){	
		vals[i] = 0;
		bool ok = (bool)files[i].read(reinterpret_cast<char*>(&vals[i]), sizeof(int));
		if(ok)add.push_back(vals[i]);

		while(files[i]){
			read_i = true;
			for(int j = i + 1; j < files.size(); j++){
			
				if(toRead[j] && files[j]){
					read_i = false;
					vals[j] = 0;
					bool ok = (bool)files[j].read(reinterpret_cast<char*>(&vals[j]), sizeof(int));
					if(vals[j] > vals[i] && ok)
						toRead[j] = false;
					else if(ok)
						add.push_back(vals[j]);
					
				}
							
			}
			if(read_i){
				
				sort(add.begin(), add.end());
				o.write(reinterpret_cast<const char*>(add.data()), add.size() * sizeof(int));
				count += add.size();

				add.clear();
				
				vals[i] = 0;					
				bool ok = (bool)files[i].read(reinterpret_cast<char*>(&vals[i]), sizeof(int));
				if(ok){
					add.push_back(vals[i]);
				}
				for(int k = i + 1; k < files.size(); k++){
					if(!toRead[k] && vals[k] <= vals[i]){
						toRead[k] = true;
						add.push_back(vals[k]);
					}	
				}	
				read_i = false;
			}
		}
	}
	for(int k = 0; k < files.size(); k++) if(!toRead[k]) add.push_back(vals[k]);
		
	sort(add.begin(), add.end());
	o.write(reinterpret_cast<const char*>(add.data()), add.size() * sizeof(int));

}



