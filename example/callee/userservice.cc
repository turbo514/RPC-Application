#include<iostream>
#include<string>
#include"user.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"
class UserService :public fixbug::UserServiceRpc
{
public:
    bool Login(std::string name,std::string pwd)
    {
        std::cout<<"doing local service Login\n";
        std::cout<<"username: "<<name<<" password: "<<pwd<<"\n";
        return true;
    }
    bool Register(uint32_t id,std::string name,std::string pwd)
    {
        std::cout<<"doing local service Register\n";
        std::cout<<"id: "<<id<<" username: "<<name<<" password: "<<pwd<<"\n";
        return true;
    }
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        //解析参数
        std::string name=request->name();
        std::string pwd=request->pwd();
        
        //业务处理
        bool loginResult=Login(name,pwd);

        //写回响应
        fixbug::ResultCode *code=response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(loginResult);

        //执行回调操作
        done->Run();
    }

    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done) override   
    {
        uint32_t id=request->id();
        std::string name=request->name();
        std::string pwd=request->pwd();

        bool registerResult=Register(id,name,pwd);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        response->set_success(registerResult);

        done->Run();
    }
};
int main(int argc,char* argv[])
{
    //1.整个框架的初始化
    MprpcApplication::Init(argc,argv);
    
    //2.框架的服务提供者
    //eg:把UserService发布到Rpc节点上
    RpcProvider provider;
    provider.NotifyService(new UserService());
    
    //3.启动一个rpc服务发布节点
    //进程阻塞，等待远程的rpc调用请求
    provider.Run();

    return 0;
}