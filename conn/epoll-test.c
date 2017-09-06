#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <string.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>

#include <queue>
#include <string>

const unsigned int serverPort = 8080;
const char *serverIP = "127.0.0.1";
//const unsigned int serverPort = 3000;
//const char *serverIP = "172.16.189.22";
const char *sendMsg = "GET /\r\n";
const int maxNum = 1024;
epoll_event evs[maxNum];
typedef void(*FuncPtr)(int  fd,void* data); //定义函数指针类型  
std::string defaultMsg = "hello world\n";


struct MsgBlock{
    //char Msg[maxNum];
    int fd;
    char Msg[maxNum];
    FuncPtr CallBackFunc;
};

void callbackFunc(int fd,void* data){
     bzero(data,1024);
     read(fd, (char*)data, 1024);
     printf("recv: %s \n",data);
}

std::queue<struct MsgBlock> MsqQue;

void printerrno()
{
    printf("errno:%d , error: %s\n", errno, strerror(errno));
}

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

bool epoll_reset(int fd)
{

    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
    epoll_ctl(fd, EPOLL_CTL_MOD, fd, &event);
}

int epoll_set_inout(int epfd,int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

int epoll_set_inout_callback(int epfd,int fd)
{
    struct epoll_event event;
    event.data.ptr = new MsgBlock;
    void* ptr = event.data.ptr;
    ((MsgBlock*)ptr)->fd = fd;
    ((MsgBlock*)ptr)->CallBackFunc = callbackFunc;

    event.events = EPOLLIN | EPOLLOUT;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);

}

int epoll_set_in(int epfd,int fd)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);
}

int epoll_set_in_callback(int epfd,int fd)
{
    struct epoll_event event;
    event.data.ptr = new MsgBlock;
    void* ptr = event.data.ptr;
    ((MsgBlock*)ptr)->fd = fd;
    ((MsgBlock*)ptr)->CallBackFunc = callbackFunc;

    event.events = EPOLLIN;
    epoll_ctl(epfd, EPOLL_CTL_MOD, fd, &event);

}

int epoll_add_callback(int epfd,int fd,int epoll_ev, FuncPtr callbackFunc, bool enable_et, bool oneshot) {
    struct epoll_event event;
    event.events = epoll_ev;
    event.data.ptr = new MsgBlock;
    void* ptr = event.data.ptr;
    ((MsgBlock*)ptr)->fd = fd;
    ((MsgBlock*)ptr)->CallBackFunc = callbackFunc;


    if (enable_et)
    {
        event.events |= EPOLLET;
    }
    if (oneshot)
    {
        event.events |= EPOLLONESHOT;
    }

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        printf("epoll add error\n");
        printerrno();
    }

}

int epoll_add(int epfd, int fd, int epoll_ev, bool enable_et, bool oneshot)
{

    struct epoll_event event;
    event.data.fd = fd;
    event.events = epoll_ev;
    if (enable_et)
    {
        event.events |= EPOLLET;
    }
    if (oneshot)
    {
        event.events |= EPOLLONESHOT;
    }

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) < 0)
    {
        printf("epoll add error\n");
        printerrno();
    }
}

int epoll_del(int epfd, int fd)
{

    epoll_event event;
    event.data.fd = fd;
    if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &event) < 0)
    {
        printf("epoll del error\n");
        printerrno();
    }
}

int main()
{
    //struct MsgBlock msg_block = {defaultMsg.c_str(),callbackFunc};
    //MsqQue.push(msg_block);
    int epfd, connSocket;
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);
    server.sin_addr.s_addr = inet_addr(serverIP);

    connSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connSocket < 0 ) {
        printf("create socket err\n");
        printerrno();
    }

    setnonblock(connSocket);
    if (connect(connSocket, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        if (errno != EINPROGRESS) {
            printf("conn error\n");
            printerrno();
        }
    }
    epfd = epoll_create1(0);
    if (epfd < 0)
    {
        printf("create epoll fd error\n");
        printerrno();
    }
    //epoll_add(epfd, connSocket, EPOLLOUT, false , false);
    epoll_add_callback(epfd, connSocket, EPOLLOUT,callbackFunc, false , false);

    while (1)
    {
        int ret = epoll_wait(epfd, evs, maxNum, 1);
        if (ret < 0)
        {
            printf("epoll wait error\n");
            printerrno();
        }

        for (int i = 0; i < ret; i++)
        {
            //printf("in loop %d\n",i);
            int sockfd = evs[i].data.fd;
            int connfd;
            if (evs[i].events & EPOLLERR){
                break;
            }
            if (evs[i].events & EPOLLIN)
            {
               printf("in recv block\n");
               void* ptr = evs[i].data.ptr;
               int fd = ((MsgBlock*)ptr)->fd;
               void* data = ((MsgBlock*)ptr)->Msg;
               ((MsgBlock*)ptr)->CallBackFunc(fd,data);
                //char buf[1024];
                //read(evs[i].data.fd, buf, 1024);
                //printf("recv: %s \n",buf);
                //bzero(buf, 1024);
                //epoll_set_inout(epfd,connSocket);
                epoll_set_inout_callback(epfd,connSocket);
               
            }
            if (evs[i].events & EPOLLOUT)
            {
                printf("in send block\n");
                if (send(connSocket, sendMsg, strlen(sendMsg)+1, MSG_DONTWAIT) < 0)
                {
                    printf("send error\n");
                    printerrno();
                }
                epoll_set_in_callback(epfd,connSocket);
                //epoll_set_in(epfd,connSocket);
               
            }
        }
    }

    return 0;
}

