#pragma once

#include"mprpcconfig.h"
#include"mprpcchannel.h"
#include"mprpccontroller.h"
#include"logger.h"

//mprpc框架的基础类，负责一些初始化
class MprpcApplication
{
public:
    static void Init(int argc,char* argv[]);
    static MprpcApplication& GetInstance();
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig _config;
    MprpcApplication(){}
    MprpcApplication(const MprpcApplication&)=delete;
    MprpcApplication(MprpcApplication&&)=delete;
};