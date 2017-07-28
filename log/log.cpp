#include "log.h"

#ifdef DEBUG

int main() {

  // auto logger = *new Logger(LOG_LEVEL_INFO,"/tmp/tmp.log",150);

  logger->Debugf("log module test:%s%d%s\n", "DEBUG", "INFO", "WARN");
  logger->Infof("log module test:%s%d%s\n", "INFO", "WARN", "ERROR");

  for (int i = 0; i < 100; i++) {
    logger->Infof("No.%d: log module test:%s%d%s\n", i, "INFO", "WARN",
                  "ERROR");
    printf("No.%d\n logged\n", i);
    sleep(1);
  };
  // logger.Log(LOG_LEVEL_INFO,"log module Log test:%s\n","LOG");
}

#endif // DEBUG

Logger *Logger::loggerInstance_ = NULL;
 

class Logger;

Logger::Logger(enum LogLevel that_log_threshold, const char *that_log_file,
               const int that_log_buffer_size) 
    : log_threshold_(that_log_threshold),log_file_(that_log_file), log_buffed_size_(0),
      log_buffer_size_(that_log_buffer_size) {}
  
int Logger::Init() {
  // log_threshold_ = that_log_threshold;
  // open的参数 a
  // Open for appending (writing at end of file).  、
  // The file is created if it does not exist.  
  // The stream is positioned at the end of the file.
  FILE *file_ptr = fopen(log_file_.c_str(), "a");
  if (file_ptr != NULL) {
    log_file_ptr_ = file_ptr;
  } else {
    fprintf(stderr, "open file %s error: %s\n", log_file_.c_str(), strerror(errno));
  }
}

Logger *Logger::GetInstance() {


  return Logger::loggerInstance_;
}

int Logger::Log(enum LogLevel log_level, const char *format, va_list ap) {

  if (log_level >= log_threshold_) {
    int n = vfprintf(log_file_ptr_, format, ap);
    log_buffed_size_ += n;
    if (log_buffed_size_ >= log_buffer_size_) {
      fflush(log_file_ptr_);
      log_buffed_size_ = 0;
    }

    return n;
  }
  return 0;
}

int Logger::Debugf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = Log(LOG_LEVEL_DEBUG, format, ap);
  va_end(ap);
  return n;
}

int Logger::Infof(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = Log(LOG_LEVEL_INFO, format, ap);
  va_end(ap);
  return n;
}

int Logger::Warnf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = Log(LOG_LEVEL_WARN, format, ap);
  va_end(ap);
  return n;
}

int Logger::Errorf(const char *format, ...) {
  va_list ap;
  va_start(ap, format);
  int n = Log(LOG_LEVEL_ERROR, format, ap);
  va_end(ap);
  return n;
}
