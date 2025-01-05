#pragma once
#include"google/protobuf/service.h"
#include<muduo/net/TcpServer.h>
#include<muduo/net/EventLoop.h>
#include<muduo/net/InetAddress.h>
#include<muduo/net/TcpConnection.h>
#include<unordered_map>
#include"google/protobuf/descriptor.h"
#include"logger.h"

//用于发布rpc服务的网络类型类
class RpcProvider
{
public:
    //发布某特定rpc服务的接口
    void NotifyService(google::protobuf::Service* service);
    //启动rpc服务节点，开始提供rpc远程网络调用类
    void Run();
private:
    muduo::net::EventLoop _eventloop;

    //服务类型信息
    struct ServiceInfo
    {
        google::protobuf::Service* _service;   //服务对象
        std::unordered_map<std::string,const google::protobuf::MethodDescriptor*> _methodMap;   //服务方法的名字与描述符
    };

    //存储注册成功的服务对象与其所有服务方法信息
    std::unordered_map<std::string,ServiceInfo> _serviceMap;

    //closure的回调操作，用于序列化rpc的响应和网络放松
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&,google::protobuf::Message*);
};