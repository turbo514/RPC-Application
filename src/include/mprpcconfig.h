#pragma once

#include<unordered_map>
#include<string>

//rpcserverip   rpcserverport   zookeeperip zookeeperport
//框架读取配置文件类
//存储有各类网络信息
class MprpcConfig
{
public:
    //解析加载配置项信息
    void LoadConfigFile(const char* configFile);
    //查询配置项信息
    std::string Load(const std::string& key);
private:
    std::unordered_map<std::string,std::string> _configMap;
    void Trim(std::string& str);
};