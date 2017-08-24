#include "config.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>

#define DEBUG
#ifdef DEBUG

struct KeyValuePair{
    std::string key_;
    std::string value_;
};


int main()
{
    Config config_parser;
    config_parser.Init("/home/hepanchen/Desktop/src/c-redis-cli/config/config.ini");
    
    //printf("%s\n",config_parser.GetByKey("log","log.level"));
    //printf("%s\n",config_parser.GetByKey("log","log.path"));
    //printf("%s\n",config_parser.GetByKey("server","server.ip"));
    //printf("%s\n",config_parser.GetByKey("server","server.port"));
    config_parser.Dump();

}
#endif



char* TrimSpace(char* src){
    if(src == NULL){
        return NULL;
    }
    char* left_ptr = src;
    char* right_ptr = src;
    while(*right_ptr != '\0' && *right_ptr != '\n'){
        if (*left_ptr == *right_ptr && *right_ptr != ' ' ) {
            left_ptr++;
            right_ptr++;
        }else if (*left_ptr != *right_ptr && *right_ptr != ' ') {
            *left_ptr++ = *right_ptr++;
        }else {
            right_ptr++;
        }
    }
    *left_ptr = '\0';
    src[1023] = '\0';
    return src;
}

char* TrimComment(char* src){
    if(src == NULL){
        return NULL;
    }
    char* tmp_ptr = src;
    while(*tmp_ptr != '\0'){
        if (*tmp_ptr == '#'){
            *tmp_ptr++ = '\n';
            *tmp_ptr = '\0';
        }
        tmp_ptr++;
    }
    return src;
}

char* Trim(char* src){
    TrimComment(src);
    TrimSpace(src);
    return src;
}

Config::Config():
    config_file_(""),
    is_inited_(false)
{}

Config::~Config(){}

// 0 true
// -1 false
// -2 NULL str
int IsSection(const char* src){
    if (src == NULL || *src == '\0') {
        return -2;
    }
    if (*src == '['){
        return 0;
    }
    return -1;
}

char* ParseSection(char* src){
    src++;
    char* left  = src;
    char* right = src;
    bool overSection = false;
    while (*right != '\0'){
    //while (*right != '\0' && *right != '\n'){
        if(!overSection && *right != ']'){
            right++;
        }else if(!overSection && *right == ']'){
            *right = '\0';
            overSection = true;
            right++;
        }else{
            return NULL;
        }
    }
    return src;
}

KeyValuePair ParseKeyValue(char* src){
    struct KeyValuePair k;
    k.key_ = "";
    k.value_ = "";
    if(src == NULL){
        return k;
    }
    char* key = src;
    char* value = src;
    char* value_right = src;
    bool over_equal = false;
    while(*value_right!='\0'){
        if(!over_equal && *value_right!='='){
            value++;
            value_right++;
        }else if(!over_equal && *value=='='){
            *value++ = '\0';
            value_right++;
            over_equal = true;
        //}else if (over_equal && *value_right=='\n'){
        }else if (over_equal){
            *value_right = '\0';
        }else{
            value_right++;
        }
    }
    k.key_=key;
    k.value_=value;
    return k;

}


int Config::Init(const char* config_file){
    if (config_file == NULL){
        exit(1);
    }
    config_file_ = config_file;
    FILE* config_file_ptr = fopen(config_file_.c_str(),"rb");
    if (config_file_ptr == NULL){
        fprintf(stderr, "open file %s error: %s\n", config_file_.c_str(), strerror(errno));
        exit(1);
    }

    std::string current_section = "";
    bool parseOver = false;
    enum {
        UnKownType = 0,
        InSection  = 1,
        InKeyValue = 2
    };
    int current_status = UnKownType;
    char* section = NULL;
    struct KeyValuePair kv;

    while(!feof(config_file_ptr)){
        if(fgets(line_buffer,kDefaultLineSize,config_file_ptr)!=NULL){
            Trim(line_buffer);
            //printf("%s",line_buffer);
            parseOver = false;
            current_status = UnKownType;

            while(!parseOver){

                //printf("DEUBG: current_status %d\n",current_status);
                //printf("DEUBG: current_section %s\n",current_section.c_str());
                switch(current_status){
                    case UnKownType:
                        if(IsSection(line_buffer)==0){
                            current_status = InSection;
                        }else if (current_section != "" && *line_buffer != '\0') {
                            current_status = InKeyValue;
                        }else {
                            parseOver = true;
                        }
                    break;

                    case InSection:
                        section = ParseSection(line_buffer);
                        //printf("DEBUG: parsed section:%s\n",section);
                        if (section!=NULL){
                            current_section = section;
                        }
                        parseOver = true;
                    break;

                    case InKeyValue:
                        //printf("DEBUG: current_section:%s\n",current_section.c_str());
                        kv = ParseKeyValue(line_buffer);
                        config_storage_[current_section+"_"+kv.key_] = kv.value_;
                        parseOver = true;
                    break;
                }

            }
            

        }
    } 

    return 0;
}

const char* Config::GetByKey(const char* section,const char* key){
    if (!is_inited_){
        fprintf(stderr, "error: not inited\n");
        exit(1);
    }
    std::map<std::string,std::string>::iterator it;
    it = config_storage_.find(std::string(section)+"_"+key);

    if (it != config_storage_.end()) {
        return it->second.c_str();
    }

    return "";
}


void Config::Dump(){
    std::map<std::string,std::string>::iterator i;
    for(i=config_storage_.begin();i!=config_storage_.end();i++)
    {
        printf("[%s]%s\n",i->first.c_str(),i->second.c_str());
    }
}
