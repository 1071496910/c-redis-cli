#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "el.h"

typedef void(*FuncPtr)(void* data); //定义函数指针类型  

void printerrno()
{
    printf("errno:%d , error: %s\n", errno, strerror(errno));
}


El::El() {
    epfd_ = epoll_create1(0);
    if (epfd_ < 0)
    {
        printf("create epoll fd error\n");
        printerrno();
        exit(1);
    }
}

int El::AddIOReadEvent(int fd,struct EventHandler* eventHandler){
    struct epoll_event event;
    int opt = EPOLL_CTL_ADD;
    event.events = EPOLLIN;
    event.data.fd = fd;

    listened_events_[fd].eventHandler = eventHandler;

    if (listened_events_[fd].listened){
        event.events |= listened_events_[fd].events;
        listened_events_[fd].events = event.events;
        opt = EPOLL_CTL_MOD;
    } else {
        listened_events_[fd].events = EPOLLIN;
        listened_events_[fd].listened = 1;

    }

    if (epoll_ctl(epfd_, opt, fd, &event) < 0)
    {
        printf("epoll add error\n");
        printerrno();
        return -1;
    }
    return 0;
    
}


int El::AddIOWriteEvent(int fd,struct EventHandler* eventHandler){
    //printf("DEBUG in add io write\n");
    struct epoll_event event;
    int opt = EPOLL_CTL_ADD;
    event.events = EPOLLOUT;
    event.data.fd = fd;

    listened_events_[fd].eventHandler = eventHandler;

    if (listened_events_[fd].listened){
        event.events |= listened_events_[fd].events;
        listened_events_[fd].events = event.events;
        opt = EPOLL_CTL_MOD;
    } else {
        listened_events_[fd].events = EPOLLOUT;
        listened_events_[fd].listened = 1;

    }

    if (epoll_ctl(epfd_, opt, fd, &event) < 0)
    {
        printf("epoll add error\n");
        printerrno();
        return -1;
    }
    return 0;
    
}

int El::DelIOWriteEvent(int fd){

    if (listened_events_[fd].listened){
        listened_events_[fd].events &= ~(EPOLLOUT);
        struct epoll_event event;
        int opt = EPOLL_CTL_MOD;
        event.events = listened_events_[fd].events;
        event.data.fd = fd;


        if (epoll_ctl(epfd_, opt, fd, &event) < 0)
        {
            printf("epoll add error\n");
            printerrno();
            return -1;
        }
        return 0;
    }
    return -1;
    
}

int El::MainLoop(){
    while(1)
    {
        //sleep(1);
        struct epoll_event evs[maxEventsNum];
        int ret = epoll_wait(epfd_, evs, maxEventsNum, -1);
        int fd;
        //printf("DEBUG: after apoll_wait\n");
        if (ret < 0)
        {
            //printf("epoll wait error\n");
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
               //printf("in recv block\n");
               //printf("get evs%d\n",evs[i].data.fd);
               //printf("after get evs\n");
               //fd = evs[i].data.fd;
               //printf("DEBUG: before process callback\n");
               listened_events_[fd].eventHandler->rFunc();
               //printf("DEBUG: after process callback\n");

               
            }
            if (evs[i].events & EPOLLOUT)
            {
                //printf("in send block\n");
                fd = evs[i].data.fd;
                listened_events_[fd].eventHandler->wFunc();

            }
        }
    }

}
