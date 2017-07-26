#ifndef C_REDIS_CLI_LOG_LOG_H_
#define C_REDIS_CLI_LOG_LOG_H_

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>

const int kDefualtLogBufferSize = 4096;
void FlushLogInBuffer(void);

enum LogLevel {
  LOG_LEVEL_DEBUG = 0,
  LOG_LEVEL_INFO = 1,
  LOG_LEVEL_WARN = 2,
  LOG_LEVEL_ERROR = 3
};

class Logger {
  int log_threshold_;
  FILE *log_file_ptr_;
  int log_buffed_size_;
  int log_buffer_size_;

public:
  static Logger *loggerInstance_;

  static Logger *
  GetInstance(enum LogLevel that_log_threshold, const char *that_log_path,
              const int that_log_buffer_size = kDefualtLogBufferSize);

public:
  int Debugf(const char *format, ...);
  int Infof(const char *format, ...);
  int Warnf(const char *format, ...);
  int Errorf(const char *format, ...);

public:
  FILE *GetLogFilePtr() { return this->log_file_ptr_; }

private:
  Logger(enum LogLevel that_log_threshold, const char *that_log_path,
         const int that_log_buffer_size = kDefualtLogBufferSize);
  Logger(const Logger &);
  Logger &operator=(const Logger &);

private:
  int Log(enum LogLevel log_level, const char *format, va_list ap);
};

Logger *Logger::loggerInstance_ = NULL;

Logger *logger = Logger::GetInstance(LOG_LEVEL_INFO, "/tmp/tmp.log");

void FlushLogInBuffer(void);

void SignalHandler(int signal_no);

#endif //C_REDIS_CLI_LOG_LOG_H_