#pragma once
#include <iostream>
#include <hiredis/hiredis.h>
#include <thread>
#include <functional>
using namespace std;

class Redis
{
public:
    Redis();
    ~Redis();

    bool connnect();    //连接redis服务器

    bool publish(int channel, string message);  //向redis指定通道channel发布消息

    bool subscribe(int channel);    //向redis指定通道channel订阅消息

    bool unsubscribe(int channel);  //向redis指定通道channel取消订阅

    void observer_channel_message();    //处理redis异步订阅回复消息

    void init_notify_handler(function<void(int,string)> fn);   //初始化消息回调函数
private:
    redisContext *publish_context_; //hiredis同步上下文对象，负责publish消息
    redisContext *subscribe_context_;    //hiredis同步上下文对象，负责subscribe消息

    function<void(int,string)> notify_message_handler_; //回调操作，收到订阅消息，给service层上报
};