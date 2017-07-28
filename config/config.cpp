#include "config.h"


int main()
{
    ConfigParser config_parser;
    config_parser.Init("/home/hpc/src/c-redis-cli/config/config.ini");
    printf("%s\n",config_parser.GetByKey("log","log.level"));
    printf("%s\n",config_parser.GetByKey("log","log.path"));
    printf("%s\n",config_parser.GetByKey("server","server.ip"));
    printf("%s\n",config_parser.GetByKey("server","server.port"));
    //config_parser.Dump();

}

ConfigParser::ConfigParser():
    config_storage_(NULL),
    config_file_(""),
    file_buffer_(NULL),
    current_section_("")
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

//Return NULL 如果section
char* ParseSection(char* s){
    if (s==NULL){
        return NULL;
    }
    bool isValid = false;
    s = TrimFront(++s);//去除[这里的空格section]
    char *tmp = s;
    while(tmp!=NULL&&*tmp!='\0'&&*tmp!='\n'){
        if(*tmp==']'){
            *tmp='\0';
            isValid = true;
        } else if (*tmp==' ') {   //去除[section这里的空格]
            *tmp='\0';
        }
        tmp++;
    }
    if (isValid == false){
        fprintf(stderr, "parse file error: session %s is invalid\n", s);
        return NULL;
    }
    //printf("DEBUG: ParseSection %s\n",s);
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
    
    (*config_storage_)[current_section_+"_"+key] = value;
    return 0;
}

int ConfigParser::ParseRaw(char* raw){

    
    //printf("DEBUG: before trim comment:%s\n",raw);
    raw = TrimComment(raw);
    //printf("DEBUG: after trim comment:%s\n",tmp);
    raw = TrimFront(raw);
    //printf("DEBUG: after trim:%s\n",tmp);
    //in session
    if(*raw == '['){
        raw = ParseSection(raw);
        if(!IsEmptyString(raw)){
            current_section_ = raw;
        }
    }else{ //in key value
        if (current_section_ != ""){
            ParseKeyValue(raw);
        }
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
    if (file_buffer_ != NULL) {
        delete file_buffer_;
        file_buffer_ = NULL;
    }
    return 0;
}

const char* ConfigParser::GetByKey(const char* section,const char* key){
    return (*config_storage_)[std::string(section)+"_"+key].c_str();
}


void ConfigParser::Dump(){
    std::map<std::string,std::string>::iterator i;
    for(i=config_storage_->begin();i!=config_storage_->end();i++)
    {
        printf("[%s]%s\n",i->first.c_str(),i->second.c_str());
    }
}