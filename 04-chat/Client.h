
#ifndef CLIENT_H
#define CLIENT_H

#include <sys/epoll.h>
#include <queue>

class Client {
    struct WriteTask {
        char* buf;
        size_t size;
        size_t written;
    };

    int efd; // epoll клиента
    int socket; // сокет клиента
    std::queue<WriteTask> output; // очередь всех тасков

public:
    Client();
    Client(int efd, int socket);

    int write_out(char* buf, size_t len);
    int flush();
};

#endif // CLIENT_H