#include"mprpcconfig.h"

#include<iostream>
#include<string>

void MprpcConfig::LoadConfigFile(const char* configFilePath)
{
    FILE* fp=fopen(configFilePath,"r");
    if(nullptr==fp)
    {
        std::cout<<configFilePath<<" is not exist"<<std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(fp))
    {
        char buff[512]={0};
        fgets(buff,sizeof buff,fp);
        std::string srcBuff(buff);
        //去除多余空格
        Trim(srcBuff);
        if(srcBuff[0]=='#'||srcBuff.empty())
        {
            continue;
        }
        int idx=srcBuff.find('=');
        if(idx==-1)
        {
            //配置项不合法
            continue;
        }
        std::string key=srcBuff.substr(0,idx);
        Trim(key);
        int idx2=srcBuff.find('\n',idx);
        std::string value=srcBuff.substr(idx+1,idx2-idx-1);
        Trim(value);
        _configMap.insert({key,value});
    }
}

std::string MprpcConfig::Load(const std::string& key)
{
    auto it=_configMap.find(key);
    if(it==_configMap.end())
        return "";
    return it->second;
}

void MprpcConfig::Trim(std::string& str)
{
    int idx = str.find_first_not_of(' ');
        if (idx != std::string::npos)
        {
            str.erase(0, idx);
            idx = str.find_last_not_of(' ');
            //if (idx != std::string::npos)
            str.erase(idx + 1);
        }
        else
            str.clear();
}
