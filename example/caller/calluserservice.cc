#include"mprpcapplication.h"
#include<iostream>
#include"user.pb.h"
#include"mprpcchannel.h"

int main(int argc,char* argv[])
{
    MprpcApplication::Init(argc,argv);
    
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());
    fixbug::LoginRequest request;
    request.set_name("zhang san");
    request.set_pwd("123456");
    fixbug::LoginResponse response;
    stub.Login(nullptr,&request,&response,nullptr);

    if(0==response.result().errcode())
    {
        std::cout<<"rpc login response success: "<<response.success()<<'\n';
    }
    else
    {
        std::cout<<"rpc login response error: "<<response.result().errmsg()<<std::endl;
    }

    fixbug::RegisterRequest registerRequest;
    registerRequest.set_id(13);
    registerRequest.set_name("lisi");
    registerRequest.set_pwd("654321");
    fixbug::RegisterResponse registerResponse;
    
    stub.Register(nullptr,&registerRequest,&registerResponse,nullptr);

    if(0==registerResponse.result().errcode())
    {
        std::cout<<"rpc register response success: "<<registerResponse.success()<<'\n';
    }
    else
    {
        std::cout<<"rpc register response error: "<<registerResponse.result().errmsg()<<std::endl;
    }
}