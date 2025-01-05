#pragma once
#include"lockqueue.h"
#include<string>

// Mprpc日志系统
enum class LogLevel
{
    INFO, //普通信息
    ERROR, //错误信息
};
class Logger
{
public:
    static Logger& GetInstance();
    void SetLogLevel(LogLevel);
    void Log(std::string);
private:
    LogLevel _logLevel;
    LockQueue<std::string> _lckQue;

    Logger();
    Logger(const Logger&)=delete;
    Logger(Logger&&)=delete;
};

//snprintf会顺便写入'\0'，可以和fputs配合使用

#define LOG_INFO(logmsgformat,...) \
    do \
    { \
        Logger& logger=Logger::GetInstance(); \
        logger.SetLogLevel(LogLevel::INFO); \
        char c[1024]; \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__); \
        logger.Log(c); \
    } while(0);

#define LOG_ERR(logmsgformat,...) \
    do \
    { \
        Logger& logger=Logger::GetInstance(); \
        logger.SetLogLevel(LogLevel::ERROR); \
        char c[1024]; \
        snprintf(c,1024,logmsgformat,##__VA_ARGS__); \
        logger.Log(c); \
    } while(0);
