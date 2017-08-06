#include <iostream>

#include "config.h"

int main()
{
    ConfigParser config_parser;
    config_parser.Init("/home/hepanchen/Desktop/src/c-redis-cli/config/config.ini");
    printf("%s\n",config_parser.GetByKey("log.level").c_str());
    printf("%s\n",config_parser.GetByKey("log.path").c_str());
    printf("%s\n",config_parser.GetByKey("server.ip").c_str());
    printf("%s\n",config_parser.GetByKey("server.port").c_str());

}

ConfigParser::ConfigParser():
    config_storage_(NULL),
    config_file_(""),
    file_buffer_(NULL)
{}

ConfigParser::~ConfigParser(){
    if (config_storage_ != NULL){
        delete config_storage_; 
    }
    if (file_buffer_ != NULL){
        delete file_buffer_; 
    }
}

int ConfigParser::Init(const char* config_file){
    config_file_ = config_file;
    config_storage_ = new std::map<std::string,std::string>;
    ReadFile();
    ParseFile();
    return 0;
}

int ConfigParser::ReadFile(){
    FILE* file_ptr = fopen(config_file_.c_str(),"rb");
    if (file_ptr == NULL){
        fprintf(stderr, "open file %s error: %s\n", config_file_.c_str(), strerror(errno));
        exit(1);
    }

    /* 获取文件大小 */  
    fseek (file_ptr , 0 , SEEK_END);  
    int file_size = ftell (file_ptr);  
    rewind (file_ptr);  

    file_buffer_ = (char*) malloc (sizeof(char)*file_size);  
    if (file_buffer_ == NULL)  
    {  
        fprintf(stderr, "malloc memory error: %s\n", strerror(errno));
        exit (2);  
    }

    /* 将文件拷贝到buffer中 */  
    int readed_size = fread (file_buffer_,1,file_size,file_ptr);  
    if (readed_size != file_size)  
    {  
        fprintf(stderr, "read file %s error: %s\n", config_file_.c_str(), strerror(errno));
        exit (3);  
    }

  
    fclose (file_ptr);  
    //printf("DEBUG: in readfile:%s\n",file_buffer_);
    return 0;  

}

char* TrimFront(char* s){
    while(s!=NULL && *s != '\0') {
        if (*s != ' ') {
            break;
        }
        s++;
    }
    return s;
}

char* TrimComment(char* s){
    char* tmp = s;
    while(tmp!=NULL&&*tmp!='\0'&&*tmp!='\n'){
        if(*tmp=='#'){
            *tmp = '\0';
            break;
        }
        tmp++;
    }
    return s;
}

bool IsEmptyString(const char* s){
    return s == NULL || *s == '\0';
}
char* ParseSession(char* s){
    bool isValid = false;
    char* tmp = ++s;
    while(tmp!=NULL&&*tmp!='\0'&&*tmp!='\n'){
        if(*tmp==']'){
            *tmp='\0';
            isValid = true;
        }
        tmp++;
    }
    if (isValid == false){
        fprintf(stderr, "parse file error: session %s is invalid\n", s);
        return NULL;
    }
    //printf("DEBUG: parseSession %s\n",s);
    return s;
}

int ConfigParser::ParseKeyValue(char* s){
    if(s == NULL){
        return -1;
    }
    bool isValid = false;
    bool hasEqualSymbol = false;
    bool hasValue = false;
    char* key = s;
    char* tmp = s;
    char* value = NULL;
    //key loop
    while(*tmp!='\0'&&*tmp!='\n'){
        if(*tmp==' '||*tmp=='='){
            if(*tmp == '='){
                hasEqualSymbol = true;
                *tmp++ ='\0';
                break;
            }else{
                *tmp++ = '\0';
            }
        }else{
            tmp++;
        }
    }
    
    //value loop
    tmp = TrimFront(tmp);
    char* tmp2 = tmp;
    while(*tmp2!='\0'){
        if(*tmp2!='#'&&*tmp2!='\n'&&*tmp2!=' '&&*tmp2!='\0'){
            tmp2++;
        }else {
            if(*tmp2!='\0'){
                *tmp2='\0';
            }
            break;
        }
        hasValue = true;
        value = tmp;
    }
    isValid = hasValue && hasEqualSymbol;
    if (! isValid){
    //    fprintf(stderr, "parse file error: value: %s is invalid\n", s);
        return 0;
    }
    //printf("[%s]%s\n",key,value);
    (*config_storage_)[key] = value;
    return 0;
}

int ConfigParser::ParseRaw(char* raw){

    std::string raw_status;
    std::string session;

    char* tmp;
    //printf("DEBUG: before trim comment:%s\n",raw);
    tmp = TrimComment(raw);
    //printf("DEBUG: after trim comment:%s\n",tmp);
    tmp = TrimFront(tmp);
    //printf("DEBUG: after trim:%s\n",tmp);
    //in session
    if(*tmp == '['){
        tmp = ParseSession(tmp);
        if(tmp != NULL){
            session = tmp;
        }
    }else{ //in key value
        ParseKeyValue(tmp);
    }
    return 0;
}

char* NextRaw(char* s){
    //printf("DEBUG at next raw%s\n",s);
    if (s==NULL){
        return NULL;
    }
    while(*s!='\n'&&*s!='\0'){
        s++;
    }
    //printf("DEBUG at next raw%s\n",s);
    if(*s=='\n'){
        return ++s;
      //  printf("Next Raw%s\n",s);
    }
    return NULL;
}

int ConfigParser::ParseFile(){
    char * raw = file_buffer_;
    while(1)
    {
        //printf("DEBUG: in parseFile\n");
        char* next_raw=NextRaw(raw);
        //printf("DEBUG: next raw:\n%s",next_raw);
        //printf("%s\n",raw);
        ParseRaw(raw);
        //printf("DEBUG: after parse next raw%s\n",next_raw);
        raw = next_raw;
        if (raw == NULL){
            break;
        }
    }
}

const std::string& ConfigParser::GetByKey(const char* key){
    return (*config_storage_)[key];
}


