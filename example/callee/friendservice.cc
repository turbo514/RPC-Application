#include<iostream>
#include<string>
#include<vector>
#include"friend.pb.h"
#include"mprpcapplication.h"
#include"rpcprovider.h"

class FriendService :public fixbug::FriendServiceRpc
{
    std::vector<std::string> GetFriendList(uint32_t id)
    {
        std::cout<<"do GetFriendList service local; userid: "<<id<<std::endl;
        std::vector<std::string> friendList;
        
        friendList.emplace_back("zhang san");
        friendList.emplace_back("Li si");
        friendList.emplace_back("wang wu");
        
        return friendList;

    }
    void GetFriendList(::google::protobuf::RpcController* controller,
                       const ::fixbug::GetFriendListRequest* request,
                       ::fixbug::GetFriendListResponse* response,
                       ::google::protobuf::Closure* done) override
    {
        uint32_t id=request->userid();

        auto friendList=GetFriendList(id);

        response->mutable_result()->set_errcode(0);
        response->mutable_result()->set_errmsg("");
        for(const std::string& name: friendList)
        {
            std::string* p=response->add_friends();
            *p=name;
        }

        done->Run();
    }
};

int main(int argc,char* argv[])
{
    MprpcApplication::Init(argc,argv);

    RpcProvider provider;
    provider.NotifyService(new FriendService());

    provider.Run();
}