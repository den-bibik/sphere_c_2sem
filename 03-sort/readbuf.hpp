#include <fstream>
#include <string>
using namespace std;

class readbuf{
	ifstream file;
	int* buf;
	size_t bufsize;
	int* point;
	int* end;
	size_t bytes_to_end;
public:
	readbuf(string name, size_t bufsize_);
	bool read_to_buf();
	bool read(int &to);
};
