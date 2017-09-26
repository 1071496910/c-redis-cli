
#include <string>
#include <queue>
#include <hash_map>

#include <../conn/el/el.h>

const int maxBufferSize = 1024;

typedef void (*callBackFunc)(void* privateData);
typedef int  (*attachFunc)(void* el);

struct Command{
  std::string command_;
  callBackFunc recvCallBack_;

};

class Client;

class ClientEventHandler : public EventHandler {

    Client* client_;

public:
    int rFunc();
    int wFunc();
    int eFunc();
}

class Client{
    char* encodeBuffer_;
    char* decodeBuffer_;
    std::queue<Command> commandQueue_;
    callBackFunc disConnCallBack_;
    callBackFunc onConnCallBack_;
    El el_;
    int connSocket_;
    ClientEventHandler eventHandler_;
    
public:
    int init(const char* ip,int port);
    int SetDisConnCallBack(callBackFunc callback);
    int SetOnConnCallBack(callBackFunc callback);
    int AsyncCommand(const char* command,callBackFunc getmsg); 

    int AwaysRun();

};

int Client::init(const char* ip,int port) {
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip);

    connSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connSocket < 0 ) {
        printf("create socket err\n");
        printerrno();
        exit(1);
    }

    setnonblock(connSocket);
    eventHandler_.client_ = this;
}

int AsyncCommand
