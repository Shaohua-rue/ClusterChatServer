#include "chatServer.h"
#include "TcpServer.h"
#include "EventLoop.h"
#include "chatService.h"

#include <signal.h>

//处理服务器ctrl + c 结束后，重置user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();
    exit(0);
}

int main(int argc,char *argv[])
{
    signal(SIGINT,resetHandler);
    if(argc<2)
    {
        std::cout<<"error:ip_address port_number"<<std::endl;
        InetAddress addr(1200,"192.168.6.129");
        EventLoop loop;
        ChatServer server(&loop,addr,"ChatServer");
        server.start();
        loop.loop();
    }
    else
    {
        InetAddress addr(atoi(argv[1]),"192.168.6.129");
        EventLoop loop;
        ChatServer server(&loop,addr,"ChatServer");
        server.start();
        loop.loop();
    }
}