#pragma once

#include<zookeeper/zookeeper.h>
#include<string>

class zkClient
{
public:
    zkClient();
    ~zkClient();
    //启动zk客户端，连接zk服务器
    void Start();
    //创建zk节点
    void Create(const char* path,const char* data,int datalen,int state=0);
    //获取zk结点的值
    std::string GetData(const char* path);
private:
    //zk的客户端句柄
    zhandle_t* _zhandle;
};