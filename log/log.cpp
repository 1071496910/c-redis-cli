#include "log.h"


const char *LogLevelString[4] = {
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};





void FlushLogInBuffer(void);

const int kDefaultLogThreshold = LOG_LEVEL_INFO;
std::string kDefualtLogFile = "/dev/stderr";
const int kDefaultBufferSize = 128;

typedef struct tm timeinfo;

#define DEBUG
#ifdef DEBUG

int main() {

  Logger::GetInstance()->Init("/dev/stderr",LOG_LEVEL_INFO);

  for (int i = 0; i < 100; i++) {
    printf("%d\n",i);
    LOG_ERROR("No.%d: log module test:%s%d%s\n", i, "INFO", "WARN","ERROR");
    sleep(1);
  };
}

#endif // DEBUG

Logger *Logger::loggerInstance_ = NULL;
 

class Logger;

Logger::Logger() 
    : log_threshold_(kDefaultLogThreshold),
      log_file_(kDefualtLogFile),
      log_file_ptr_(NULL),
      log_buffed_size_(0),
      log_buffer_size_(kDefaultBufferSize) 
    {}
  
int Logger::Init(const char* log_file,enum LogLevel log_level) {
  if(log_file != NULL) {
    log_file_ = log_file;
  }

  FILE *file_ptr = fopen(log_file_.c_str(), "a");
  if (file_ptr != NULL) {
    log_file_ptr_ = file_ptr;
  } else {
    fprintf(stderr, "open file %s error: %s\n", log_file_.c_str(), strerror(errno));
    file_ptr = fopen(kDefualtLogFile.c_str(),"a");
    fprintf(stderr, "use stderr as log output\n");
  }
}

Logger *Logger::GetInstance() {
  if(Logger::loggerInstance_ == NULL){
      Logger::loggerInstance_ = new Logger;
  }

  return Logger::loggerInstance_;
}

int Logger::Log(enum LogLevel log_level,const char* file,const int line,const char* func,const char *format, ...) {

    struct timeval currentTime;
    gettimeofday(&currentTime,NULL);
    tm *timeinfos = localtime(&(currentTime.tv_sec));   
    fprintf(log_file_ptr_,"[%d-%d-%d %d:%d:%d:%ld] %s:%d %s [%s] ",  
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
                    LogLevelString[log_level]
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

int Logger::LevelUp(){
  if (log_threshold_ < LOG_LEVEL_ERROR) {
    log_threshold_++;
  }
}

int Logger::LevelDown(){
  if (log_threshold_ > LOG_LEVEL_DEBUG) {
    log_threshold_--;
  }
}

//Rotate 只会重新打开日志文件
int Logger::Rotate(){
  if (log_file_ptr_ != NULL) {
    fclose(log_file_ptr_);
    log_file_ptr_ = NULL;
  }
  log_file_ptr_ =  fopen(log_file_.c_str(),"a");
  if (log_file_ptr_ == NULL) {
    fprintf(stderr, "open file %s error: %s\n", log_file_.c_str(), strerror(errno));
    return -1;
  }
  return 0;
}
