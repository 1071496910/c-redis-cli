#include "log.h"


const char *LogLevelString[4] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};


#define LOGDEBUG(fmt,...) \
  do {                                                                                  \
    if ( LOG_LEVEL_DEBUG >= Logger::GetInstance()->GetLogThreshold()){                  \
        Logger::GetInstance()->Log(__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__);   \
    }                                                                                   \
  }while(0)

#define LOGINFO(fmt,...) \
  do {                                                                                  \
    if ( LOG_LEVEL_INFO >= Logger::GetInstance()->GetLogThreshold()){                   \
        Logger::GetInstance()->Log(__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__);   \
    }                                                                                   \
  }while(0)

#define LOGWARN(fmt,...) \
  do {                                                                                  \
    if ( LOG_LEVEL_WARN >= Logger::GetInstance()->GetLogThreshold()){                   \
        Logger::GetInstance()->Log(__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__);   \
    }                                                                                   \
  }while(0)

#define LOGERROR(fmt,...) \
  do {                                                                                  \
    if ( LOG_LEVEL_ERROR >= Logger::GetInstance()->GetLogThreshold()){                  \
        Logger::GetInstance()->Log(__FILE__,__LINE__,__FUNCTION__,fmt,##__VA_ARGS__);   \
    }                                                                                   \
  }while(0)


void FlushLogInBuffer(void);

const int kDefaultLogThreshold = LOG_LEVEL_INFO;
std::string kDefualtLogFile = "/tmp/tmp.log";
const int kDefaultBufferSize = 128;

typedef struct tm timeinfo;

#define DEBUG
#ifdef DEBUG

int main() {

  Logger::GetInstance()->Init();

  for (int i = 0; i < 100; i++) {
    printf("%d\n",i);
    LOGINFO("No.%d: log module test:%s%d%s\n", i, "INFO", "WARN","ERROR");
    sleep(1);
  };
}

#endif // DEBUG

Logger *Logger::loggerInstance_ = NULL;
 

class Logger;

Logger::Logger() 
    : log_threshold_(kDefaultLogThreshold),
      log_file_(kDefualtLogFile),
      log_buffed_size_(0),
      log_buffer_size_(kDefaultBufferSize) {}
  
int Logger::Init() {
  FILE *file_ptr = fopen(log_file_.c_str(), "a");
  if (file_ptr != NULL) {
    log_file_ptr_ = file_ptr;
  } else {
    fprintf(stderr, "open file %s error: %s\n", log_file_.c_str(), strerror(errno));
  }
}

Logger *Logger::GetInstance() {
  if(Logger::loggerInstance_ == NULL){
      Logger::loggerInstance_ = new Logger;
  }

  return Logger::loggerInstance_;
}

int Logger::Log(const char* file,const int line,const char* func,const char *format, ...) {

    struct timeval currentTime;
    gettimeofday(&currentTime,NULL);
    tm *timeinfos = localtime(&(currentTime.tv_sec));   
    fprintf(log_file_ptr_,"[%d-%d-%d %d:%d:%d:%d] %s:%d %s [%s] ",  
                    timeinfos->tm_year+1900,    
                    timeinfos->tm_mon+1,       
                    timeinfos->tm_mday,  
                    timeinfos->tm_hour,  
                    timeinfos->tm_min,  
                    timeinfos->tm_sec,
                    currentTime.tv_usec,
                    file,
                    line,
                    func,
                    LogLevelString[LOG_LEVEL_INFO]
                    );

    va_list ap;
    va_start(ap,format);
    int n = vfprintf(log_file_ptr_, format, ap);
    va_end(ap);
    log_buffed_size_ += n;
    if (log_buffed_size_ >= log_buffer_size_) {
      fflush(log_file_ptr_);
      log_buffed_size_ = 0;
    }

    return n;
}

