#ifndef C_REDIS_CLI_CONFIG_CONFIG_H_
#define C_REDIS_CLI_CONFIG_CONFIG_H_

#include <map>
#include <string>


const int kDefaultLineSize = 1024;
class  Config {

    std::map<std::string,std::string> config_storage_;
    std::string config_file_;
    char line_buffer[kDefaultLineSize];
    bool is_inited_;

public:
    Config();
    ~Config();

public:
    int Init(const char* config_file);
    const char* GetByKey(const char* section,const char* key);
    void Dump();

};


#endif //C_REDIS_CLI_CONFIG_CONFIG_H_