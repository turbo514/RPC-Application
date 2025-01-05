#include"rpcprovider.h"
#include"mprpcapplication.h"
#include "rpcheader.pb.h"
#include"zookeeperutil.h"
void RpcProvider::NotifyService(google::protobuf::Service* service)
{
    ServiceInfo serviceInfo;
    //获取服务对象的描述
    const google::protobuf::ServiceDescriptor* pserviceDesc  = service->GetDescriptor();
    //获取服务的名字
    std::string serviceName=pserviceDesc->name();
    //获取服务对象的方法数量
    int methodCnt = pserviceDesc->method_count();

    serviceInfo._service=service;
    for(int i=0;i<methodCnt;i++)
    {
        const google::protobuf::MethodDescriptor* pmethodDesc=pserviceDesc->method(i);
        serviceInfo._methodMap.emplace(pmethodDesc->name(),pmethodDesc);
    }
    _serviceMap.emplace(serviceName,serviceInfo);
}

void RpcProvider::Run()
{
    std::string ip=MprpcApplication::GetConfig().Load("rpcserverip");
    std::string port = MprpcApplication::GetConfig().Load("rpcserverport");
    muduo::net::InetAddress addr(ip,atoi(port.c_str()));
    muduo::net::TcpServer server(&_eventloop,addr,"RpcProvider");

    //连接回调
    server.setConnectionCallback([this](const muduo::net::TcpConnectionPtr& conn){
        if(!conn->connected())
            conn->shutdown();
        return;
    });
    
    //已建立连接的用户的读写事件回调,回应远程rpc请求
    /**
     * headerSize(4),headerStr,argsStr
     * headerStr:serviceName,methodName,argsSize
     */
    server.setMessageCallback([this](const muduo::net::TcpConnectionPtr& conn,muduo::net::Buffer* buff,muduo::Timestamp ts){
        std::string recvBuff=buff->retrieveAllAsString();
        uint32_t headerSize;
        recvBuff.copy((char*)&headerSize,4,0);

        //读取数据头的原始字符流，反序列化数据，得到rpc请求的详细信息
        mprpc::RpcHeader rpcHeader;
        const std::string& rpcHeaderStr=recvBuff.substr(4,headerSize);
        if(!rpcHeader.ParseFromString(rpcHeaderStr))
        {
            std::cout<<"rpcHeaderStr: "<<rpcHeaderStr<<" parse error!"<<std::endl;
            return;
        }
        const std::string& serviceName=rpcHeader.servicename();
        const std::string& methodName=rpcHeader.methodname();
        uint32_t argsSize=rpcHeader.argssize();
        const std::string& argsStr=recvBuff.substr(4+headerSize,argsSize);



        std::cout<<"=========================================================\n";
        std::cout<<"headerSize: "<<headerSize<<'\n';
        std::cout<<"rpcHeaderStr: "<<rpcHeaderStr<<'\n';
        std::cout<<"serviceName: "<<serviceName<<'\n';
        std::cout<<"methodName: "<<methodName<<'\n';
        std::cout<<"argsSize: "<<argsSize<<'\n';
        std::cout<<"========================================================="<<std::endl;
        
        //找到要调用的服务和方法
        auto it=this->_serviceMap.find(serviceName);
        if(it==this->_serviceMap.end())
        {
            std::cout<<serviceName<<" not exist!"<<std::endl;
            return;
        }
        auto mit=it->second._methodMap.find(methodName);
        if(mit==it->second._methodMap.end())
        {
            std::cout<<methodName<<" not exist!"<<std::endl;
            return;
        }
        google::protobuf::Service* service=it->second._service;
        const google::protobuf::MethodDescriptor* methodDesc=mit->second;

        //生成rpc方法调用需要的请求request和响应response
        google::protobuf::Message* request=service->GetRequestPrototype(methodDesc).New();
        google::protobuf::Message* response=service->GetResponsePrototype(methodDesc).New();
        if(!request->ParseFromString(argsStr))
        {
            std::cout<<"argsStr: "<<argsStr<<" parse error!"<<std::endl;
            return;
        }

        //绑定回调函数(序列化，并发送给caller)
        google::protobuf::Closure* done=
        google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
        (this,&RpcProvider::SendRpcResponse,conn,response);

        //
        service->CallMethod(methodDesc,nullptr,request,response,done);
        return;
    });
    server.setThreadNum(4);

    //将当前rpc结点上的服务注册到zk上，让rpc client可以从zk上发现服务
    zkClient zkCli;
    zkCli.Start();
    for(const auto& service: _serviceMap)
    {
        std::string service_path="/"+service.first;
        zkCli.Create(service_path.c_str(),nullptr,0);
        for(const auto& method:service.second._methodMap)
        {   
            std::string method_path=service_path+"/"+method.first;
            char method_path_data[128]={0};
            sprintf(method_path_data,"%s:%s",ip.c_str(),port.c_str());
            zkCli.Create(method_path.c_str(),method_path_data,strlen(method_path_data),ZOO_EPHEMERAL);
        }
    }

    std::cout<<"RpcProvider start service at ip:"<<ip<<" port: "<<port<<std::endl;
    server.start();
    _eventloop.loop();

}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,google::protobuf::Message* response)
{
    //序列化
    std::string responseStr;
    if(response->SerializeToString(&responseStr))
    {
        //序列化成功后，将rpc服务结果发送回caller
        conn->send(responseStr);
    }
    else
    {
        //序列化失败
        std::cout<<"serialize responseStr error!"<<std::endl;
    }
    conn->shutdown();  //模拟http的短链接服务，rpcProvider服务方主动断开
}
