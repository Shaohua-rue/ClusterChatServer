#pragma once

#include <iostream>
#include <functional>
#include <string>

#include "TcpServer.h"
#include "EventLoop.h"

class ChatServer
{
public:
    //初始化聊天服务器对象
    ChatServer(EventLoop *loop,
               const InetAddress &listenAddr,
               const std::string &nameArg);

    //启动服务
    void start();

private:
    //上报连接相关信息回调函数
    void onConnection(const TcpConnectionPtr &conn);

    //上报读写事件相关信息回调函数
    void onMessage(const TcpConnectionPtr &conn,
                   Buffer *buffer,
                   Timestamp time);
    EventLoop *loop_;
    TcpServer server_;
};


