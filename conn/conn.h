#ifndef C_REDIS_CLI_CONN_CONN_H_
#define C_REDIS_CLI_CONN_CONN_H_

#include <string>
#include <queue>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>


const int maxEventsNum = 1024;
int setnonblock(int fd)
{

    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    if (fcntl(fd, F_SETFL, new_option) < 0)
    {
        printf("setnoblock error\n");
        printerrno();
        return -1;
    }
    return old_option;
}

void printerrno()
{
    printf("errno:%d , error: %s\n", errno, strerror(errno));
}

int epoll_set_inout(int epfd,int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}


typedef void(*FuncPtr)(int socket); //定义函数指针类型  

struct SendBlock{
    std::string msg_;
    int conn_socket_;
    FuncPtr get_msg_func;
}

class AsyncConn{
    int conn_socket_;
    int conn_port_;
    std::string conn_ip_;
    std::queue<struct SendBlock> send_queue_;


public:
    
    AsyncConn(const char* conn_ip,int conn_port);

    int Connect();
    int Send(const char* send_str,FuncPtr get_msg_callback);
    int Wait();
};

AsyncConn::AsyncConn(const char* conn_ip,int conn_port)
        :conn_socket_(-1),
         conn_ip_(conn_ip),
         conn_port_(conn_port),
         conn_func_(NULL),
         disconn_func_(NULL)

{}

int AsyncConn::Connect(){
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(conn_port_);
    server.sin_addr.s_addr = inet_addr(conn_ip_.c_str());

    conn_socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (conn_socket_ < 0 ) {
        printf("create socket err\n");
        printerrno();
    }

    setnonblock(conn_socket_);
    if (connect(conn_socket_, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        if (errno != EINPROGRESS) {
            printf("conn error\n");
            printerrno();
            exit(1);
        }
    }

}

int AsyncConn::Send(const char* send_str,FuncPtr get_msg_callback){
    struct SendBlock msg_block = {send_str,get_msg_callback};
    send_queue_.push(msg_block);
}

int AsyncConn::Wait(){
    epoll_event evs[maxEventsNum];
    epfd = epoll_create1(0);
    if (epfd < 0)
    {
        printf("create epoll fd error\n");
        printerrno();
    }
    epoll_set_inout(epfd,conn_socket_);
    while(1)
    {
        int ret = epoll_wait(epfd, evs, maxEventsNum, -1);
        if (ret < 0)
        {
            printf("epoll wait error\n");
            printerrno();
        }

        for (int i = 0; i < ret; i++)
        {
            if (evs[i].events & EPOLLERR)
            {
                break;
            }
            if (evs[i].events & EPOLLIN)
            {
               printf("in recv block\n");
               SendBlock* ptr = (SendBlock*)(evs[i].data.ptr);
               ptr->get_msg_func(ptr->conn_socket_);
               epoll_set_inout(epfd,conn_socket_);
               delete ptr;
               
            }
            if (evs[i].events & EPOLLOUT)
            {
                printf("in send block\n");
                if (!send_queue_.empty()){
                    struct SendBlock send_block_ = send_queue_.front();
                    struct epoll_event event;
                    event.data.ptr = new SendBlock;
                    SendBlock* ptr = (SendBlock*)(event.data.ptr);
                    ptr->msg_ = send_block_.msg_;
                    ptr->get_msg_func = send_block_.get_msg_func;
                    ptr->conn_socket_ = conn_socket_;
                    event.events = EPOLLIN;
                    epoll_ctl(epfd, EPOLL_CTL_MOD, conn_socket_, &event); 
                    send_queue_.pop();
                }
            }
        }
    }


}

#endif //C_REDIS_CLI_CONN_CONN_H_
