#ifndef C_REDIS_CLI_CONFIG_CONFIG_H_
#define C_REDIS_CLI_CONFIG_CONFIG_H_

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct LogConfig
{
    char level[6];
    char path[1024];

    struct LogConfig Initialize(const char *that_level, const char *that_path);
};

LogConfig LogConfig::Initialize(const char *that_level, const char *that_path)
{
    memcpy(this->level, that_level, strlen(that_level) + 1);
    memcpy(this->path, that_path, strlen(that_path) + 1);
    return *this;
}

struct Config
{
    struct LogConfig config_log;

    struct Config Initialize(const struct LogConfig that_log);
};

struct Config Config::Initialize(const struct LogConfig that_log)
{
    this->config_log.Initialize(that_log.level, that_log.path);
}

struct Config ParseConfigFile(const char *file_name)
{
    FILE *file_ptr = fopen(file_name, r);
    if (filt_ptr == NULL)
    {
        fread(fileptr,fpstate())
    }
    return NULL;
}

#endif //C_REDIS_CLI_CONFIG_CONFIG_H_