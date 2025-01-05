#include"zookeeperutil.h"
#include"mprpcapplication.h"
#include<semaphore.h>
#include<iostream>
//zkServer给zkClient发通知,观察者模式的update函数
void global_watcher(zhandle_t *zh, int type,int state, const char *path,void *watcherCtx)
{
    //客户端与服务器重新连接或失去连接时触发
    if(type==ZOO_SESSION_EVENT)
    {
        sem_t* sem=(sem_t*)zoo_get_context(zh);
        sem_post(sem);
    }
}

zkClient::zkClient():_zhandle(nullptr)
{
}

zkClient::~zkClient()
{
    //关闭句柄
    if(_zhandle!=nullptr)
        zookeeper_close(_zhandle);
}

void zkClient::Start()
{
    std::string host=MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
    std::string port=MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
    std::string connStr=host+":"+port;

    //初始化句柄
    _zhandle=zookeeper_init(connStr.c_str(),global_watcher,30000,nullptr,nullptr,0);
    if(_zhandle==nullptr)
    {
        std::cout<<"zookeeper_init error"<<std::endl;
        exit(EXIT_FAILURE);
    }
    sem_t sem;
    sem_init(&sem,0,0);
    zoo_set_context(_zhandle,&sem);

    //等待响应
    sem_wait(&sem);
    std::cout<<"zookeeper_init success!\n";
}



void zkClient::Create(const char* path,const char* data,int datalen,int state)
{
    char path_buff[128];
    int bufflen = sizeof path_buff;
    int flag;
    //如果结点已存在，则不重复创建
    flag=zoo_exists(_zhandle,path,0,nullptr);
    if(ZNONODE==flag)
    {
        flag=zoo_create(_zhandle,path,data,datalen,&ZOO_OPEN_ACL_UNSAFE,state,path_buff,bufflen);
        if(flag==ZOK)
        {
            LOG_INFO("create zknode successfully,path:%s",path);
        }
        else
        {
            LOG_ERR("create zknode error,path:%s ,state:%d",path,state);
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        LOG_ERR("Znode already exists, path:%s",path);
    }
}
std::string zkClient::GetData(const char* path)
{
    char buff[64];
    int bufflen=sizeof buff;
    int flag=zoo_get(_zhandle,path,0,buff,&bufflen,nullptr);
    if(flag!=ZOK)
    {
        LOG_ERR("get zknode error,path:%s",path);
        return "";
    }
    else
        return buff;
}