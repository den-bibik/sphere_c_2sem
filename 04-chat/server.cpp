#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <list>
#include <time.h>
#include <netinet/in.h>
#include <errno.h>
#include <set>
#include <stdlib.h>
#include <string.h>
#include <map>

using namespace std;

const int PORT = 3100;
const int EVENT_BUF_SIZE = 1000;
const int TCP = 6;
const int msgBufSize = 1000;

const char* HELLO_MSG = "Welcome\n";

class Server{
public:
	Server(int port, int event_buf_size);
	void run();
	~Server();
private:
	int set_nonblock(int fd);
	void new_client(epoll_event&);

	std::set <int> clients;
	struct sockaddr_in addr;
	int epoll_fd;
	int listener;
	char *msgBuf;
	struct epoll_event *events;
};

int Server::set_nonblock(int fd){
    int flags;
#if defined(O_NONBLOCK)
    if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
        flags = 0;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
    flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

Server::Server(int port, int event_buf_size){
	msgBuf = new char[msgBufSize];

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htons(INADDR_ANY);

	listener = socket(AF_INET, SOCK_STREAM, 0);
	if(listener < 0) {
		cout << "listener create error " << listener << endl;
		exit(0);
	}
	set_nonblock(listener);

	int optval = 1;
    setsockopt(listener, TCP, SO_REUSEADDR, &optval, sizeof(optval));
    int res = bind(listener, (struct sockaddr *) &addr, sizeof(addr));
    if(res < 0) {
    	cout << "bind to listener error" << endl;
    	exit(0);
    }
    res = listen(listener, event_buf_size);
    if(res < 0) {
    	cout << "listen error" << endl;
    	exit(0);
    }

    epoll_fd = epoll_create1(0);
    if(epoll_fd < 0) {
    	cout << "epool create learning error" << endl;
    	exit(0);
    }
    struct epoll_event ev;
    ev.data.fd = listener;
    ev.events = EPOLLIN;

    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listener, &ev);
    
    events = new epoll_event[EVENT_BUF_SIZE];

}

void Server::new_client(epoll_event& event){
	cout << "nc"  << std::flush;
	int client = accept(listener, 0, 0);
	if(client < 0) {
    	cout << "accept to listener error" << endl;
    	exit(0);
    }
    set_nonblock(client);

    struct epoll_event ev;
    ev.data.fd = client;
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client, &ev);

    clients.insert(client);

    send(client, HELLO_MSG, sizeof(HELLO_MSG), MSG_NOSIGNAL);

}

void Server::run(){
	int events_num = epoll_wait(epoll_fd, events, EVENT_BUF_SIZE, -1);
	cout << events_num << std::flush;
	for (int i = 0; i < events_num; ++i) {
		if(events[i].data.fd == listener) 
			new_client(events[i]);
		else{
			memset(msgBuf, 0, sizeof(msgBufSize));
			int received = recv(events[i].data.fd, msgBuf, sizeof(msgBuf), MSG_NOSIGNAL);
			if (received <= 0) {
				shutdown(events[i].data.fd, SHUT_RDWR);
                close(events[i].data.fd);
                clients.erase(events[i].data.fd);
			}
			else{
				msgBuf[received] = 0;
			
	        	for (auto p:clients)
	                send(p, msgBuf, received, MSG_NOSIGNAL);
	        }
		}
	}
}

Server::~Server(){
	shutdown(listener, SHUT_RDWR);
	close(listener);
	for (auto p:clients){
		shutdown(p, SHUT_RDWR);
		close(p);
	}
	delete[] msgBuf;
	delete[] events;
}

int main(int argc, char ** argv){
	Server server(PORT, EVENT_BUF_SIZE);

	while(true) server.run();
	return 0;

}