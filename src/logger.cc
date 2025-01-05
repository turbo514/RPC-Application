#include "logger.h"
#include<time.h>
#include<iostream>

Logger &Logger::GetInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    std::thread writeLog([&](){
        for(;;)
        {
            time_t now=time(nullptr);
            tm* nowtm=localtime(&now);

            char fileName[128];
            sprintf(fileName,"%d-%d-%d-log.txt",nowtm->tm_year+1900,nowtm->tm_mon+1,nowtm->tm_mday);

            FILE* fp=fopen(fileName,"a+");
            if(fp==nullptr)
            {
                std::cout<<"open file : "<<fileName<<" error!"<<std::endl;
                exit(EXIT_FAILURE);
            }
            std::string msg=_lckQue.Pop();
            char timeBuff[128]={0};
            sprintf(timeBuff,"%d-%d-%d => [%s] ",nowtm->tm_hour,nowtm->tm_min,nowtm->tm_sec,_logLevel==LogLevel::INFO?"INFO":"ERROR");
            msg.insert(0,timeBuff);
            fputs(msg.c_str(),fp);
            fputc('\n',fp);
            fclose(fp);
        }
    });
    writeLog.detach();
}

void Logger::SetLogLevel(LogLevel loglevel)
{
    _logLevel=loglevel;
}

void Logger::Log(std::string msg)
{
    _lckQue.Push(msg);
}
