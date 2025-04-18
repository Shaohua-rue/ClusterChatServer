#include <iostream>
#include <functional>
#include <string>

#include "TcpServer.h"
#include "EventLoop.h"

class EchoServer
{
public:
    EchoServer(EventLoop *loop, const InetAddress inetAddress, std::string name):
    server_(loop,inetAddress,name,TcpServer::kReusePort),loop_(loop)
    {
        server_.setConnectionCallback(
            std::bind(&EchoServer::onConnection,this,std::placeholders::_1));
        server_.setMessageCallback(
            std::bind(&EchoServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
    }
    void start()
    {
        server_.start();
    }

private:
    void onConnection(const TcpConnectionPtr& conn)
    {
        if(conn->connected())
        {
            std::cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<" is connected"<<std::endl;
        }
        else
        {
            std::cout<<conn->peerAddress().toIpPort()<<"->"<<conn->localAddress().toIpPort()<<" is disconnected"<<std::endl;
        }
    }
    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp time)
    {
        std::string bufStr = buf->retrieveAllAsString();
        std::cout<<"recv data: "<<bufStr<<" time: "<<time.toString()<<std::endl;
        conn->send(bufStr);
    }

private:
    TcpServer server_;
    EventLoop* loop_;
};
int main(int argc, char* argv[])
{
    EventLoop loop;
    InetAddress addr(1200,"192.168.6.129");

    EchoServer server(&loop,addr,"EchoServer");

    server.start();
    loop.loop();
    return 0;
}