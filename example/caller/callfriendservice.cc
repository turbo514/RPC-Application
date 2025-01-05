#include"mprpcapplication.h"
#include<iostream>
#include"friend.pb.h"

int main(int argc,char* argv[])
{
    MprpcApplication::Init(argc,argv);
    
    fixbug::FriendServiceRpc_Stub stub(new MprpcChannel());
    fixbug::GetFriendListRequest request;
    request.set_userid(23);
    fixbug::GetFriendListResponse response;
    MprpcController controller;
    stub.GetFriendList(&controller,&request,&response,nullptr);
    if (controller.Failed())
    {
        std::cout << controller.ErrorText() << std::endl;
    }
    else
    {
        if (0 == response.result().errcode())
        {
            std::cout << "rpc login response success: " << '\n';
            for (int i = 0; i < response.friends_size(); i++)
            {
                std::cout << response.friends(i) << '\n';
            }
        }
        else
        {
            std::cout << "rpc login response error: " << response.result().errmsg() << std::endl;
        }
    }
    return 0;
}