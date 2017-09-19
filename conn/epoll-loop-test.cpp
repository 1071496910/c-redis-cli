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

#include "el/el.cpp"

const unsigned int serverPort = 8080;
const char *serverIP = "127.0.0.1";
//const unsigned int serverPort = 3000;
//const char *serverIP = "172.16.189.22";
const char *sendMsg = "GET /\r\n";
const int maxNum = 1024;
epoll_event evs[maxNum];
std::string defaultMsg = "hello world\n";

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

struct userBuffer{
    char readBuffer[1024];
    char writeBuffer[1024];
};


class Sender : public EventHandler  {

   struct userBuffer buffer;
   int fd;
   struct El* el;
 
public:
   int rFunc();
   int wFunc() {
   char buff[1024] = {0};
   int ret = read(s->fd, s->buffer.readBuffer, 1024);
   if (ret <= 0 ) {
       printf("read error\n");
       printerrno();
   }
   printf("recv:%s\n",s->buffer.readBuffer);
   bzero(s->buffer.readBuffer,1024);
   s->el->AddIOWriteEvent(s->fd,writeFunc,s);

   }
   int eFunc();

};

void writeFunc(void* data);

void readFunc(void* data){
   struct Sender* s = (struct Sender*)data;
   char buff[1024] = {0};
   int ret = read(s->fd, s->buffer.readBuffer, 1024);
   if (ret <= 0 ) {
       printf("read error\n");
       printerrno();
   }
   printf("recv:%s\n",s->buffer.readBuffer);
   bzero(s->buffer.readBuffer,1024);
   s->el->AddIOWriteEvent(s->fd,writeFunc,s);
}

void writeFunc(void* data){
   
   struct Sender* s = (struct Sender*)data;
   if (send(s->fd, sendMsg, strlen(sendMsg)+1, MSG_DONTWAIT) < 0)
   {
       printf("send error\n");
       printerrno();
   }
   printf("DEBUG: after send\n");
   s->el->AddIOReadEvent(s->fd,readFunc,s);
   s->el->DelIOWriteEvent(s->fd);
   
}

int main()
{
    int connSocket;
    struct Sender* sender = new Sender;
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

    sender->fd = connSocket;

    El eloop;
    sender->el = &eloop;
    eloop.AddIOWriteEvent(connSocket,writeFunc,sender);
    printf("DEBUG: after add write\n");
    
    eloop.MainLoop();

    return 0;
}

