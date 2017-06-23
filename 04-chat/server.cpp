#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

#include <map>
#include <vector>
#include <fstream>

#include "Client.h"

#define MAXEVENTS 256
#define MAX_MSG_CHUNK 4096

/* сделаем неблокирующий сокет. (блокирующий - ждём, пока всё не передастся) */
int nonblocking(int sock_fd) {
    /* F_GETFL - читаем флаги файлового дескриптора  */
    int flags = fcntl(sock_fd, F_GETFL, 0);

    if (flags == -1) {
        perror ("nonblocking : fcntl : F_GETFL");
        return -1;
    }

    flags |= O_NONBLOCK;
    
    /* F_GETFL - Устанавливаем часть флагов, относящихся к состоянию файла, как неблокирующий  */
    int s = fcntl(sock_fd, F_SETFL, flags); 

    if (s == -1) {
        perror ("nonblocking : fcntl : F_SETFL");
        return -1;
    }

    return 0;
}

int create_master(uint port) {
    int socket_fd;

    /* стандартная структура sockaddr_in из <netinet/in.h>
     * struct sockaddr_in {
     *      short            sin_family;   // семейство адресов (? константа, которая определяет, который протокол используем IP4 ?)
     *      unsigned short   sin_port;     // Переменная sin_port содержит порт сокета в сетевом порядке байт (16 бит)
     *      struct in_addr   sin_addr;     // то, куда мы будем передавать
     *      char             sin_zero[8];  // Нужна, чтобы размер структуры не менялся в зависимости от протокола 
     * };
     */
    struct sockaddr_in socket_addr;

    /* инициализируем нулями */
    bzero(&socket_addr, sizeof(socket_addr));

    socket_addr.sin_family = AF_INET; // всегда AF_INET
    socket_addr.sin_port = htons(port); // host to network shot - convert port (unsigned short) to network byte order

    /* INADDR_LOOPBACK - наш IP */
    socket_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // convert local address (unsigned long) to network byte order

    /* Создаём конечную точку соединения и получем файловый дескриптор. Принимает три аргумента 
     * domain : указывающий семейство протоколов создаваемого сокета: AF_INET - IP4 
     * type : SOCK_STREAM - потоковый сокет
     * protocol : IPPROTO_TCP - определяем используемый транспортный протокол (TCP)
     */
    socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* связываем сокет с конкретным адресом: 0 - успех, -1 - ошибка */ 
    int res = bind(socket_fd, (struct sockaddr *) &socket_addr, sizeof(socket_addr)); // assign address

    if (res == -1) {
        return -1;
    }
    /* обозначаем сокет неблокирующим */
    res = nonblocking(socket_fd);
    if (res == -1) {
        return -1;
    }

    return socket_fd;
}

int main(int argc,  char** argv){

    int master_socket;
    std::vector<int> slave_sockets;
    std::map<int, Client> clients_map; 

    int port = atoi(argv[1]);
    std::cout << "PORT : "<< port << "\n";

    master_socket = create_master(port);


    if (master_socket == -1) {
        perror("main : master_socket : create_master");
        return 1;
    }

    /* listen() подготавливает привязываемый сокет к принятию входящих соединений. 
     * sockfd — корректный дескриптор сокета.
     * backlog — целое число, означающее число установленных соединений, которые могут быть обработаны в любой момент времени. 
     */
    int s = listen(master_socket, SOMAXCONN);

    if (s == -1) {
        perror("main : listen");
        return 1;
    }

    /* создаём экземпляр epoll 
    * (возвращает дескриптор файла, указывающий на новый экземпляр epoll)
    * тоже самое, что epoll_create, только значение переменной size не используется
    *
    * epoll : следит за несколькими файловыми дескрипторами и ждёт, когда станет возможен ввод-вывод с одним из них. 
    */
    int efd = epoll_create1(0); 


    /*
     * typedef union epoll_data {
     *          void    *ptr;
     *          int      fd;
     *          uint32_t u32;
     *          uint64_t u64;
     *      } epoll_data_t;
     *   
     * struct epoll_event {
     *     uint32_t     events;    // флаги, определяющие типы событий, которые мы ловим 
     *    epoll_data_t data;      // то, за чем мы следим
     * };
     */
    struct epoll_event event;
    struct epoll_event *events;

    /* определяем master_socket в качестве event 
     * EPOLLET - возвращает события, когда появились новые данные
     * (т.е. кто-то написал в сокет, мы прочитали часть, больше нас это не тревожит, пока кто-то ещё не напишет)
     * EPOLIN - определяет тип event (то, что пришли новые данные)
     */
    event.data.fd = master_socket;

    /* определяем флаги master_socket */
    event.events = EPOLLIN | EPOLLET;
    
    /* используется для управления epoll instance, 
     * добавляем файловый дескриптор к наблюдеиню
     * т.е. прикрепляем к efd(epoll) наш event 
     */
    s = epoll_ctl(efd, EPOLL_CTL_ADD, master_socket, &event);
    if (s == -1) {
        perror("main : epoll_ctl");
        return 1;
    }

    /* выделяем место для будующих соединений */
    events = new epoll_event[MAXEVENTS];

    while (true) {
        /* ожидаем события на открытый epoll efd 
         * timout = -1 : ждём бесконечно 
         */
        int n = epoll_wait(efd, events, MAXEVENTS, -1); 
        
        /* n - количество пришедших events */
        for (int i = 0; i < n; i++) {

            int fd = events[i].data.fd;
            
            /* ошибка  или он соекта закрыт
             * EPOLLOUT — файловый дескриптор готов продолжить принимать данные (для записи)
             * EPOLLERR — в файловом дескрипторе произошла ошибка
             * EPOLLHUP — закрытие файлового дескриптора
             */
            if ((events[i].events & EPOLLERR) || 
                (events[i].events & EPOLLHUP) || 
                (!(events[i].events & (EPOLLIN | EPOLLOUT)))){ // error
               
                printf("epoll error at descriptor %d\n", fd);
                /* стираем fd из клиентов */
                clients_map.erase(fd);
                close(fd);
            } 
            else 
                if ((events[i].events & EPOLLOUT) && (fd != master_socket)) { // client socket is writeable, flush data
                    /* Если EPOLLOUT, значит есть что отправить */
                    Client& c = clients_map[fd];
                    if (c.flush() == -1) { // not writeable, exclude this client from map
                        clients_map.erase(fd);
                        close(fd);
                    }
                } 
                else 
                    if (fd == master_socket) {
                        /* на мастера пришли какие-то соединения */
                        while (true) {
                            struct sockaddr in_addr;
                            socklen_t in_len;
                            int infd;
                            char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                            in_len = sizeof(in_addr);
                            
                            /* accept() используется для принятия запроса на установление соединения от удаленного хоста. 
                             * Аргументы:
                             * sockfd — дескриптор слушающего сокета на принятие соединения.
                             * cliaddr — указатель на структуру sockaddr, для принятия информации об адресе клиента.
                             * addrlen — указатель на socklen_t, определяющее размер структуры, содержащей клиентский адрес и переданной в accept(). 
                             * Когда accept() возвращает некоторое значение, socklen_t указывает сколько байт структуры cliaddr использовано в данный момент.
                             * возвращает дескриптор сокета, связанный с принятым соединением, или −1 в случае возникновения ошибки
                             */
                            infd = accept(master_socket, &in_addr, &in_len);

                            if (infd == -1) {
                                /* EAGAIN & EWOULDBLOCK - there is no data available right now, try again later 
                                 */
                                if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                                    break; 
                                }
                                else {
                                    /* всё плохо, завершаем программу */
                                    perror("main : while : fd == master_socket :  while : accept\n");
                                    delete[] events;
                                    close(master_socket);
                                    exit(1);
                                }
                            }
                            /* делаем дескриптор неблокирующим */
                            nonblocking(infd);

                            /* добавляем клиента в slave_sockets */
                            slave_sockets.push_back(infd);

                            /* переводим имя адреса в имя машины 
                             * NI_NUMERICHOST - имя машины возвращается в числовой форме. 
                             * NI_NUMERICSERV - имя сервиса возвращается в числовой форме, например по номеру его порта. 
                             * возвращает 0, если всё хорошо
                             */
                            s = getnameinfo(&in_addr, in_len,
                                            hbuf, sizeof(hbuf),
                                            sbuf, sizeof(sbuf),
                                            NI_NUMERICHOST | NI_NUMERICSERV);

                            if (s == 0) {
                                printf("New connection (descriptor: %d, host: %s, port: %s)\n", infd, hbuf, sbuf);
                            }

                            event.data.fd = infd;
                            event.events = EPOLLIN | EPOLLET;
                            s = epoll_ctl(efd, EPOLL_CTL_ADD, infd, &event);
                            if (s == -1) {
                                perror("main : while : fd == master_socket :  while : epoll_ctl");
                                return 1;
                            }
                            /* создаём клиент*/
                            clients_map[infd] = Client(efd, infd);
                        }
                    } 
                    else { /* пришло новое сообщение */
                        bool closed = false;
                        /* читаем сообщение  chunk-by-chunk */ 
                        while (true) {
                            ssize_t count;
                            char buf[MAX_MSG_CHUNK];
                            /* считываем chunk из сокета */
                            count = read(fd, buf, sizeof(buf)); // count - сколько считали
                            if (count == -1) {
                                if (errno != EAGAIN) {
                                    perror("main : while : fd != master_socket : while : read");
                                    closed = true;
                                }
                            break;
                            }
                            else 
                                if (count == 0) { // EOF; connection closed
                                    closed = true;
                                    break;
                                }

                            /* пытаемся отправить всем клиентам пришедшее сообщение */
                            for (std::pair<const int, Client> &kv : clients_map){
                                int result = kv.second.write_out(buf, count);
                                if (result == -1) {
                                    /* если не смогли, то удаляем и закрываем сокет */
                                    clients_map.erase(fd);
                                    close(fd);
                                }
                            }
                }
                if (closed) {
                    /* если словили ошибку или всё считали: закроем соединение */
                    printf("connection closed on descriptor %d\n", fd);
                    clients_map.erase(fd);
                    close(fd);
                }
            }
        }
    }

    delete[] events;
    close(master_socket);
}