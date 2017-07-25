#include <iostream>

#include "config.h"

int main()
{
    struct LogConfig log_config;
    struct Config config;
    config.Initialize(
        log_config.Initialize("info", "/tmp/tmp.conf"));

    std::cout << config.config_log.level << std::endl;
}