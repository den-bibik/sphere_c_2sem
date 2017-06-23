
CXX=g++
CXXFLAGS=--std=c++11

all: server

server: server.cpp Client.cpp
	$(CXX) $(CXXFLAGS) -o server server.cpp Client.cpp

clean:
	rm server