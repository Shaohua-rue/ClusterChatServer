#include "chatServer.h"
#include <functional>
#include "chatService.h"
#include "json.hpp"

using json = nlohmann::json;
//初始化聊天服务器对象
ChatServer::ChatServer(EventLoop *loop,const InetAddress &listenAddr,const std::string &nameArg):
server_(loop,listenAddr,nameArg,TcpServer::kReusePort),
loop_(loop)
{
    server_.setConnectionCallback(std::bind(&ChatServer::onConnection,this,std::placeholders::_1));
    server_.setMessageCallback(std::bind(&ChatServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    server_.setThreadNum(4);
}

void ChatServer::start()
{
    server_.start();
}

void ChatServer::onConnection(const TcpConnectionPtr &conn)
{
    if(conn->connected())
    {
        std::cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"state:online"<<std::endl;
    }
    else
    {
        std::cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<"state:offline"<<std::endl;
        ChatService::instance()->clientCloseException(conn);
        conn->shutdown();
        conn->send("notice:you are offline");
    }
}

void ChatServer::onMessage(const TcpConnectionPtr &conn, Buffer *buffer,Timestamp time)
{
    std::string buf=buffer->retrieveAllAsString();
    //数据的反序列化
    json js = json::parse(buf);
    //通过js["msgid"]获取消息id，然后分发给不同的handler处理
    auto msgHandle = ChatService::instance()->getHandler(js["msgid"].get<int>());
    //回调消息绑定好的事件处理器，来执行相应的业务代码
    msgHandle(conn,js,time);

}

