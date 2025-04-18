#pragma once

#include "TcpConnection.h"
#include <unordered_map>
#include <memory>
#include <mutex>

#include "json.hpp"
#include "usemodel.hpp"
#include "friendmodel.hpp"
#include "offlinemessagemodel.hpp"
#include "groupmodel.hpp"
#include "group.hpp"

#include  "redis.hpp"
using json = nlohmann::json;

//表示处理消息的事件回调类型
using MsgHandler = std::function<void(const TcpConnectionPtr &conn, json &js, Timestamp)>;

//聊天服务器业务类
class ChatService
{
public:
    // 获取单例对象的接口函数
    static ChatService *instance();

    // 处理登录任务
    void login(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注册任务
    void reg(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 一对一聊天业务
    void oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 添加好友业务
    void addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 创建群组业务
    void createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 加入群组业务
    void addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 群组聊天业务
    void groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理注销业务
    void loginout(const TcpConnectionPtr &conn, json &js, Timestamp time);
    // 处理客户端异常退出
    void clientCloseException(const TcpConnectionPtr &conn);
    // 服务器异常，业务重置方法
    void reset();
    // 获取消息对应的处理器
    MsgHandler getHandler(int msgid);

    void handleRedisSubscibeMessage(int channel, std::string message);

private:
    ChatService();

    //存储消息id与其对应业务的处理方法 <消息id, 业务处理方法>
    std::unordered_map<int, MsgHandler> msgHandlerMap_;


    //存储在线用户的通信连接 <用户id, 连接>（多线程情况下，需要互斥锁保护）
    std::unordered_map<int, TcpConnectionPtr> userConnMap_;
    std::mutex connMutex_;


    //数据操作类对象
    UserModel userModel;
    OfflineMsgModel offlineMsgModel;
    FriendModel friendModel;
    GroupModel groupModel;

    //redis操作对象
    Redis redis_;
};