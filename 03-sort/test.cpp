#include <iostream>
#include <fstream>
#include <climits>

using namespace std;
const int ERRORS = 10;
int main(int argc, char **argv){
	std::ifstream f(argv[1],std::ios::binary);
	int prev = INT_MIN, tmp = 0;
	long long int i = 0;
	
	int ok = ERRORS;

	size_t buf_len = (size_t) 50 * 1024 * 1024 / 2; 
	while(f){
		i++;
		f.read(reinterpret_cast<char*>(&tmp), sizeof(int));
		if(tmp < prev){
			ok -= 1;
			cout << "error on " << i * 4 / 1024 / 1024 << " mb" << "; block num " << (i * 4) / buf_len << 
				"; value num " << ((i * 4) % buf_len) / 4 << endl;
			cout << "tmp val: " << tmp << " prev val: " << prev << endl;
			if(!ok)	return 0;
		}
		prev = tmp;
	} 
	if(ERRORS == ok) cout << "all OK" << endl;
	return 0;
}
