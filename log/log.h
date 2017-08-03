#ifndef C_REDIS_CLI_LOG_LOG_H_
#define C_REDIS_CLI_LOG_LOG_H_

#include <string.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string>
#include <signal.h>

enum LogLevel {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_ERROR = 3
};

class Logger {
  int log_threshold_;
  std::string log_file_;
  FILE *log_file_ptr_;
  int log_buffed_size_;
  int log_buffer_size_;

  static Logger *loggerInstance_;

public:


  static Logger *GetInstance();

public:
  int Log(const char* file,const int line,const char* func,const char *format, ...);

public:
  FILE *GetLogFilePtr() { return log_file_ptr_; }
  int   GetLogThreshold() { return log_threshold_; }
  int Init();
private:
  Logger();

  Logger(const Logger &);
  Logger &operator=(const Logger &);

private:
};

#endif //C_REDIS_CLI_LOG_LOG_H_
