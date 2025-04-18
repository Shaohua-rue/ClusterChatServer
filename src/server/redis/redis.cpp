#include "redis.hpp"

Redis::Redis():publish_context_(nullptr),subscribe_context_(nullptr)
{

}
Redis::~Redis()
{
    if(publish_context_)
    {
        redisFree(publish_context_);
    }
    if(subscribe_context_)
    {
        redisFree(subscribe_context_);
    }
}

bool Redis::connnect()
{
    publish_context_ = redisConnect("127.0.0.1",6379);
    if(publish_context_ == nullptr)
    {
        cerr << "connect redis failed! "<< endl;
    }
    subscribe_context_ = redisConnect("127.0.0.1",6379);
    if(subscribe_context_ == nullptr)
    {
        cerr << "connect redis failed" << endl;
    }

    //新开辟线程，监听通道上的事件，有消息给业务层进行上报
    thread t([&](){
        observer_channel_message();
    });
    t.detach();
    cout <<  "connect redis success!" << endl; 
    return true;
}

bool Redis::publish(int channel, string message)
{
    redisReply *reply = (redisReply *)redisCommand(publish_context_,"PUBLISH %d %s",channel,message.c_str());
    if(nullptr == reply)
    {
        cerr << "publish command failed" << endl;
        return false;
    }
    freeReplyObject(reply);
    return true;
}
bool Redis::subscribe(int channel)
{
    //subscribe命令本身会造成线程阻塞等待通道里面发生消息，这里只创建订阅通道，不接受通道消息
    //通道消息的接收专门在observer_channel_message函数中的独立线程中进行
    //只负责发送命令，不阻塞接收redis_server响应消息，否则和notifyMsg线程抢占响应资源

    if(redisAppendCommand(this->subscribe_context_,"SUBSCRIBE %d", channel) == REDIS_ERR)
    {
        cerr << "subscribe command faild!" << endl;
        return false;
    } 
    //redisBudderWrite 可以循环发送缓冲区，直到缓冲区数据发送完毕(done被置1)
    int done = 0;
    while(!done)
    {
        if(redisBufferWrite(this->subscribe_context_,&done) == REDIS_ERR)
        {
            cerr << "subscibe command faid!" <<endl;
            return false;
        }
    }
    return true;
}
bool Redis::unsubscribe(int channel)
{
    if(redisAppendCommand(this->subscribe_context_,"UNSUBSCRIBE %d", channel) == REDIS_ERR)
    {
        cerr << "subscribe command faild!" << endl;
        return false;
    } 
    //redisBudderWrite 可以循环发送缓冲区，直到缓冲区数据发送完毕(done被置1)
    int done = 0;
    while(!done)
    {
        if(redisBufferWrite(this->subscribe_context_,&done) == REDIS_ERR)
        {
            cerr << "subscibe command faid!" <<endl;
            return false;
        }
    }
    return true;
}
void Redis::observer_channel_message()
{
    redisReply *reply = nullptr;

    while(redisGetReply(this->subscribe_context_,(void **)&reply) == REDIS_OK)
    {
        //订阅收到的消息是一个带三元素的数组
        if(reply != nullptr && reply->element[2] != nullptr && reply->element[2]->str != nullptr)
        {
            //给业务层上报通道上发生的消息
            notify_message_handler_(atoi(reply->element[1]->str),reply->element[2]->str);
        }
        freeReplyObject(reply);
    }  
    cerr << ">>>>>>>>>>>>>>>>>>>>>>>>>>observer_channel_message quit" << endl;
}
void Redis::init_notify_handler(function<void(int,string)> fn)
{
    this->notify_message_handler_ = fn;
}