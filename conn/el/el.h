#ifndef C_REDIS_CLI_EL_EL_H_
#define C_REDIS_CLI_EL_EL_H_


#include <hash_map>

typedef void(*FuncPtr)(void* data); //定义函数指针类型  

const int maxEventsNum = 1024;


void printerrno();
//interface
class EventHandler {
public:
    virtual int rFunc() = 0;
    virtual int wFunc() = 0;
    virtual int eFunc() = 0;
};

struct IOEvent {
    int listened;
    int events;
    EventHandler* eventHandler;
};

class El {
    int epfd_;
    //struct IOEvent listened_events_[maxEventsNum];
      struct __gnu_cxx::hash_map<int,struct IOEvent> listened_events_;
//    std::list<struct IOEvent> fired_events_;

public:
    El();
    int AddIOReadEvent(int fd, struct EventHandler* eventHandler);
    int AddIOWriteEvent(int fd, struct EventHandler* eventHandler);
    int DelIOWriteEvent(int fd);
    int AddIOErrorEvent(int fd, struct EventHandler* eventHandler);
    int MainLoop();
};

#endif //C_REDIS_CLI_EL_EL_H_
