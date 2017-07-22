#include "log.h"

// int main(){
//     auto logger = Logger(LOG_LEVEL_INFO,"/tmp/tmp.log");
//     logger.Debugf("log module test:%s%d%s\n","DEBUG","INFO","WARN");
//     logger.Infof("log module test:%s%d%s\n","INFO","WARN","ERROR");
//     //logger.Log(LOG_LEVEL_INFO,"log module Log test:%s\n","LOG");
// }

class Logger;

Logger::Logger(enum LogLevel that_log_threshold, const char* that_log_path){
    this->log_threshold = that_log_threshold;
    memcpy(this->log_path,that_log_path,strlen(that_log_path)+1);
}

int Logger::Log(enum LogLevel log_level,const char* format,va_list ap){
    //va_list ap;
    //va_start(ap,format);
    //printf("Get va list head %d\n",ap);
    if(log_level >= this->log_threshold){
        return vprintf(format,ap);
    }
    return 0;
}

int Logger::Debugf(const char* format,...){
    va_list ap;
    va_start(ap,format);
    int n = this->Log(LOG_LEVEL_DEBUG,format,ap);
    va_end(ap);
    return n;
}

int Logger::Infof(const char* format,...){
    va_list ap;
    va_start(ap,format);
    int n = this->Log(LOG_LEVEL_INFO,format,ap);
    va_end(ap);
    return n;
}

int Logger::Warnf(const char* format,...){
    va_list ap;
    va_start(ap,format);
    int n = this->Log(LOG_LEVEL_WARN,format,ap);
    va_end(ap);
    return n;
}

int Logger::Errorf(const char* format,...){
    va_list ap;
    va_start(ap,format);
    int n = this->Log(LOG_LEVEL_ERROR,format,ap);
    va_end(ap);
    return n;
}