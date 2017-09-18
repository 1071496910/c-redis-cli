#ifndef C_REDIS_CLI_CONN_CONN_H_
#define C_REDIS_CLI_CONN_CONN_H_

#include <string>
#include <queue>

typedef void(*FuncPtr)(int socket); //定义函数指针类型  

const int maxEventsNum = 1024;

class AsyncConn{
    int conn_socket_;
    int conn_port_;
    std::string conn_ip_;
    struct epoll_event evs[maxEventsNum];
    int epfd;


public:
    
    AsyncConn(const char* conn_ip,int conn_port);

    int Connect();
    int Send(const char* send_str,FuncPtr get_msg_callback);
    int Wait();
};

#endif //C_REDIS_CLI_CONN_CONN_H_
