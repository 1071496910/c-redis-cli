#ifndef C_REDIS_CLI_CONFIG_CONFIG_H_
#define C_REDIS_CLI_CONFIG_CONFIG_H_

#include <map>
#include <string>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

class  ConfigParser {

    std::map<std::string,std::string>* config_storage_;
    std::string config_file_;
    char* file_buffer_;
    std::string current_section_;

public:
    ConfigParser();
    ~ConfigParser();

public:
    int Init(const char* config_file);
    const char* GetByKey(const char* section,const char* key);
    void Dump();

private:
    int ReadFile();
    int ParseRaw(char* raw);
    int ParseKeyValue(char* s);
    int ParseFile();

};


#endif //C_REDIS_CLI_CONFIG_CONFIG_H_