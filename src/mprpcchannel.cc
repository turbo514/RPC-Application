#include"mprpcchannel.h"
#include<string>
#include<google/protobuf/descriptor.h>
#include<google/protobuf/message.h>
#include"rpcheader.pb.h"
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<arpa/inet.h>
#include"mprpcapplication.h"
#include<unistd.h>
#include"mprpccontroller.h"
#include"zookeeperutil.h"
/*
协议
rpc:
header_size(4),header_str,args_str
header_str:
service_name,method_name,args_size
*/
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    const google::protobuf::ServiceDescriptor* service=method->service();
    const std::string& service_name=service->name();
    const std::string& method_name=method->name();
    uint32_t args_size=0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        args_size=args_str.size();
    }
    else
    {
        controller->SetFailed("serialize request error");
        return;
    }
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_servicename(service_name);
    rpcHeader.set_methodname(method_name);
    rpcHeader.set_argssize(args_size);
    std::string header_str;
    uint32_t header_size=0;
    if(rpcHeader.SerializeToString(&header_str))
    {
        header_size=header_str.size();
    }
    else
    {
        controller->SetFailed("serialize header error");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.reserve(4+header_size+args_size);
    send_rpc_str.insert(0,std::string((char*)&header_size,4)); 
    send_rpc_str+=header_str;
    send_rpc_str+=args_str;

    std::cout<<"=========================================================\n";
    std::cout<<"headerSize: "<<header_size<<'\n';
    std::cout<<"rpcHeaderStr: "<<header_str<<'\n';
    std::cout<<"serviceName: "<<service_name<<'\n';
    std::cout<<"methodName: "<<method_name<<'\n';
    std::cout<<"argsSize: "<<args_size<<'\n';
    std::cout<<"========================================================="<<std::endl;

    //发送rpc请求
    int clntfd=socket(AF_INET,SOCK_STREAM,0);
    if(clntfd==-1)
    {
        char errtext[512]={0};
        sprintf(errtext,"create socket error,errno:%d",errno);
        controller->SetFailed(errtext);
        close(clntfd);
        return;
    }

    //从服务配置中心读取请求方法的地址和端口
    zkClient zkClient;
    zkClient.Start();
    std::string method_path="/"+service_name+"/"+method_name;
    std::string host_data=zkClient.GetData(method_path.c_str());
    if(host_data=="")
    {
        controller->SetFailed(method_path+" is not exists");
        return;
    }
    int idx=host_data.find(":");
    if(idx==-1)
    {
        controller->SetFailed(host_data+"the host_data is invalid");
        return;
    }
    std::string ip = host_data.substr(0,idx);
    uint16_t port = atoi(host_data.substr(idx+1).c_str());

    struct sockaddr_in serv_addr;
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(port);
    serv_addr.sin_addr.s_addr=inet_addr(ip.c_str());

    if(-1==connect(clntfd,(struct sockaddr*)&serv_addr,sizeof serv_addr))
    {
        char errtext[512]={0};
        sprintf(errtext,"connnect error,errno:%d",errno);
        controller->SetFailed(errtext);
        close(clntfd);
        return;
    }

    if(-1==send(clntfd,send_rpc_str.c_str(),send_rpc_str.size(),0))
    {
        char errtext[512]={0};
        sprintf(errtext,"send error,errno:%d",errno);
        controller->SetFailed(errtext);
        close(clntfd);
        return;
    }

    //接收rpc请求的响应值
    char buff[1024]={0};
    int recv_size=0;
    if(-1==(recv_size=recv(clntfd,buff,sizeof buff,0)))
    {
        char errtext[512]={0};
        sprintf(errtext,"receive error,errno:%d",errno);
        controller->SetFailed(errtext);
        close(clntfd);
        return;
    }
    
    //反序列化
    // std::string response_str(buff,0,recv_size);  //若字符数组有'\0'，则会提前终止
    if(!response->ParseFromArray(buff,recv_size))
    {
        char errtext[2048]={0};
        sprintf(errtext,"ParseFromArray error:%s",buff);
        controller->SetFailed(errtext);
        close(clntfd);
        return;
    }
    close(clntfd);
    
}