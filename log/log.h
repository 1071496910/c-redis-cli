#ifndef __REDIS_CLI_LOG_LOG_H
#define __REDIS_CLI_LOG_LOG_H

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

enum LogLevel {
    LOG_LEVEL_DEBUG    = 0,
    LOG_LEVEL_INFO     = 1,
    LOG_LEVEL_WARN     = 2,
    LOG_LEVEL_ERROR    = 3
};

class Logger{
    int  log_threshold;
    char log_path[1024];
public:
    Logger(enum LogLevel that_log_threshold, const char* that_log_path);
public:
    int Debugf(const char* format,...);
    int Infof(const char* format,...);
    int Warnf(const char* format,...);
    int Errorf(const char* format,...);
//private:
    int Log(enum LogLevel log_level,const char* format,va_list ap);
};



#endif //__REDIS_CLI_LOG_LOG_H