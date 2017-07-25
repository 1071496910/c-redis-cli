#include "log.h"

int main(){


    //auto logger = *new Logger(LOG_LEVEL_INFO,"/tmp/tmp.log",150);
    
    logger->Debugf("log module test:%s%d%s\n","DEBUG","INFO","WARN");
    logger->Infof("log module test:%s%d%s\n","INFO","WARN","ERROR");
    comtest = 1;
    for(int i = 0;i<100;i++){
        logger->Infof("No.%d: log module test:%s%d%s\n",i,"INFO","WARN","ERROR");
        printf("No.%d\n logged\n",i);
        sleep(1);
    };
    //logger.Log(LOG_LEVEL_INFO,"log module Log test:%s\n","LOG");
}

void FlushLogInBuffer(void){
    fclose(logger->GetLogFilePtr());
}

void SignalHandler(int signal_no){
    printf("at signal handler\n");
    fflush(logger->GetLogFilePtr());
    exit(1);
}


class Logger;

Logger::Logger(enum LogLevel that_log_threshold, const char* that_log_path, const int that_log_buffer_size):
            log_buffed_size_(0),
            log_threshold_(that_log_threshold),
            log_buffer_size_(that_log_buffer_size)
{
    //this->log_threshold_ = that_log_threshold;
    FILE* file_ptr = fopen(that_log_path,"a");
    if(file_ptr != NULL) {
        this->log_file_ptr_ = file_ptr;

    } else {
        fprintf(stderr,"open file %s error: %s\n",that_log_path,strerror(errno));
    }

}


Logger* Logger::GetInstance(enum LogLevel that_log_threshold,
                            const char* that_log_path,
                            const int that_log_buffer_size) {

    if(Logger::loggerInstance_ == NULL) {
        Logger::loggerInstance_ = new Logger(that_log_threshold,that_log_path,that_log_buffer_size);
        if(signal(SIGINT,SignalHandler) != 0){
            fprintf(stderr,"signal error: %s\n",strerror(errno));
        }
        if(signal(SIGTERM,SignalHandler) != 0){
            fprintf(stderr,"signal error: %s\n",strerror(errno));
        }
        if(signal(SIGKILL,SignalHandler) != 0){
            fprintf(stderr,"signal error: %s\n",strerror(errno));
        }
    }
    return Logger::loggerInstance_;
}

int Logger::Log(enum LogLevel log_level,const char* format,va_list ap){

    if(log_level >= this->log_threshold_){
        int n = vfprintf(this->log_file_ptr_,format,ap);
        this->log_buffed_size_ += n;
        if (this->log_buffed_size_ >= this->log_buffer_size_) {
            fflush(this->log_file_ptr_);
            this->log_buffed_size_ = 0;
        }

        return n;
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
