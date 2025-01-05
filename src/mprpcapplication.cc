#include"mprpcapplication.h"
#include<iostream>
#include<unistd.h>
#include<string>

MprpcConfig MprpcApplication::_config;

void ShowArgsHelp()
{
    std::cout<<"example:command -i <configfile>"<<'\n';
}

void MprpcApplication::Init(int argc,char* argv[])
{
    if(argc<3)
    {
        ShowArgsHelp();
        exit(EXIT_FAILURE);
    }
    int c=0;
    std::string config_file;
    while((c=getopt(argc,argv,"i:"))!=-1)
    {
        switch (c)
        {
        case 'i':
            config_file=optarg;
            break;
        case '?':
        case ':':  //?
            ShowArgsHelp();
            exit(EXIT_FAILURE);
        default:
            break;
        }
    }
    _config.LoadConfigFile(config_file.c_str());
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication instance;
    return instance;
}
MprpcConfig& MprpcApplication::GetConfig()
{
    return _config;
}
