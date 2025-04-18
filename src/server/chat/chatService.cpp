#include "chatService.h"
#include "public.hpp"
#include "Logger.h"
#include <map>
ChatService * ChatService::instance()
{
    static ChatService chatService_;
    return &chatService_;
}
ChatService::ChatService()
{
    msgHandlerMap_[LOGIN_MSG] = std::bind(&ChatService::login, this, std::placeholders::_1, std::placeholders::_2,std::placeholders::_3);
    msgHandlerMap_[REG_MSG] = std::bind(&ChatService::reg, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    msgHandlerMap_[ONE_CHAT_MSG] = std::bind(&ChatService::oneChat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    msgHandlerMap_[ADD_FRIEND_MSG] = std::bind(&ChatService::addFriend,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3);

    //群组业务管理相关事件处理回调函数
    msgHandlerMap_[CREATE_GROUP_MSG] = std::bind(&ChatService::createGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    msgHandlerMap_[ADD_GROUP_MSG] = std::bind(&ChatService::addGroup, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
    msgHandlerMap_[GROUP_CHAT_MSG] = std::bind(&ChatService::groupChat, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    msgHandlerMap_[LOGINOUT_MSG] = std::bind(&ChatService::loginout, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

    //连接redis服务器
    if(redis_.connnect())
    {
        //设置上报信息的回调
        redis_.init_notify_handler(std::bind(&ChatService::handleRedisSubscibeMessage,this,std::placeholders::_1,std::placeholders::_2));
    }
}
// 获取消息对应的处理器
MsgHandler ChatService::getHandler(int msgid)
{
    if (msgHandlerMap_.find(msgid) == msgHandlerMap_.end())
    {
       //返回一个默认处理器，做错误处理
       return [=](const TcpConnectionPtr &conn, json &js, Timestamp time) {
           LOG_ERROR("msgid:%d is not exist!", msgid);
       };
    }
    else
    {
        return msgHandlerMap_[msgid];
    }
}
// 处理登录任务
void ChatService::login(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO("do login service!");
    int id = js["id"].get<int>();
    std::string pwd = js["password"];

    User user = userModel.query(id);
    if(user.getId() == id && user.getPwd() == pwd)
    {
        if(user.getState() == "online")
        {
            // 用户已经登录，不允许重复登录
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 2;
            response["errmsg"] = "该账户已经登录，请重新登录";
            conn->send(response.dump());
        }
        else
        {
            //登录成功，记录用户连接信息
            {
                std::lock_guard<std::mutex> lock(connMutex_);
                userConnMap_[id] = conn;
            }

            //id 用户登录成功后，向redis订阅channel(id)
            redis_.subscribe(id);
            //更新用户状态信息
            user.setState("online");
            userModel.updateState(user);
            // 登录成功，构造json消息，返回给客户端
            json response;
            response["msgid"] = LOGIN_MSG_ACK;
            response["errno"] = 0;
            response["id"] = user.getId();
            response["name"] = user.getName();
           

            //查询该用户是否有离线消息
            std::vector<std::string> vec = offlineMsgModel.query(id);
            if(!vec.empty())
            {
                // 遍历这个vector，然后拼接成所有离线消息，发送给客户端
                for(auto &str : vec)
                {
                   response["offlinemsg"] = vec;
                   //读取完成后，删除用户的所有消息
                   offlineMsgModel.remove(id);
                }
            }

           

            //查询好友信息并返回
            std::vector<User> vecFriend = friendModel.query(id);
            if(!vecFriend.empty())
            {
               std::vector<std::string> vecFriendStr;
               for(auto &user : vecFriend)
               {
                 json js;
                 js["id"] = user.getId();
                 js["name"] = user.getName();
                 js["state"] = user.getState();
                 vecFriendStr.push_back(js.dump());
               }
               response["friends"] = vecFriendStr;
            }

            //查询用户的群组消息
            std::vector<Group> vecGroup = groupModel.queryGroups(id);
            if(!vecGroup.empty())
            {
                // group:[{groupid:[xxx, xxx, xxx, xxx]}]
                std::vector<std::string> groupVecStr;
                for(auto &group : vecGroup)
                {
                    json grpjson;
                    grpjson["id"] = group.getId();
                    grpjson["groupname"] = group.getName();
                    grpjson["groupdesc"] = group.getDesc();
                    std::vector<std::string>userV;
                    for(auto &groupuser : group.getUsers())
                    {
                        json subjson;
                        subjson["id"] = groupuser.getId();
                        subjson["name"] = groupuser.getName();
                        subjson["state"] = groupuser.getState();
                        subjson["role"] = groupuser.getRole();
                        userV.push_back(subjson.dump());
                    }
                    grpjson["users"] = userV;
                    groupVecStr.push_back(grpjson.dump());
                }
                response["groups"] = groupVecStr;
            }
            conn->send(response.dump());
        }
    }
    else
    {
        // 登录失败
        json response;
        response["msgid"] = LOGIN_MSG_ACK;
        response["errno"] = 1;
        response["errmsg"] = "用户名或密码错误";
        conn->send(response.dump());
    }
}

// 处理注册任务 name password
void ChatService::reg(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    LOG_INFO("do reg service!");
    std::string name = js["name"];
    std::string pwd = js["password"];

    User user;
    user.setName(name);
    user.setPwd(pwd);
    bool state = userModel.insert(user);
    if(state)
    {
        // 注册成功
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 0;
        response["id"] = user.getId();
        conn->send(response.dump());
    }
    else
    {
        // 注册失败
        json response;
        response["msgid"] = REG_MSG_ACK;
        response["errno"] = 1;
        response["id"] = user.getId();
        conn->send(response.dump());

    }

}
// 一对一聊天业务
void ChatService::oneChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int toid = js["toid"].get<int>();

    {
       std::lock_guard<std::mutex> lock(connMutex_);
       auto it = userConnMap_.find(toid);
       if(it!= userConnMap_.end())
       {
         //toid在线，同时在同一台服务器上，转发消息
         it->second->send(js.dump());
         return;
       }


       //查询toid是否在线
       User user = userModel.query(toid);
       if(user.getState() == "online")  //在线，不在一个服务器，存储到redis中
       {
            redis_.publish(toid,js.dump());
            return;
       }

        //用户不在线，存储到数据库
        offlineMsgModel.insert(toid, js.dump());

    }
}
// 添加好友业务
void ChatService::addFriend(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int friendid = js["friendid"].get<int>();

    friendModel.insert(userid, friendid);

}
// 创建群组业务
void ChatService::createGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    std::string groupname = js["groupname"];
    std::string groupdesc = js["groupdesc"];

    //存储新创建群组信息
    Group group(-1, groupname, groupdesc);
    if(groupModel.createGroup(group))
    {
        //存储群组创建人信息
        groupModel.addGroup(userid, group.getId(), "creator");
    }
}
// 加入群组业务
void ChatService::addGroup(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    groupModel.addGroup(userid, groupid, "normal");
}
// 群组聊天业务
void ChatService::groupChat(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    int groupid = js["groupid"].get<int>();
    std::vector<int> useridVec = groupModel.queryGroupUsers(userid, groupid);

    lock_guard<std::mutex> lock(connMutex_);
    for(int id : useridVec)
    {
        auto it = userConnMap_.find(id);
        if(it != userConnMap_.end())
        {
            //转发群消息
            it->second->send(js.dump());
        }
        else
        {
            //查询toid是否在线
            User user = userModel.query(id);
            if(user.getState() == "online")
            {
                redis_.publish(id,js.dump());
            }
            else{
                //存储到数据库
                offlineMsgModel.insert(id, js.dump());
            }

        }
    }

}
// 处理注销业务
void ChatService::loginout(const TcpConnectionPtr &conn, json &js, Timestamp time)
{
    int userid = js["id"].get<int>();
    {
        lock_guard<std::mutex> lock(connMutex_);
        auto it = userConnMap_.find(userid);
        if(it != userConnMap_.end())
        {
            userConnMap_.erase(it);
        }
    }
    //用户注销，相当于就是下线，在redis中取消订阅通道
    redis_.unsubscribe(userid);
    
    //更新用户的状态信息
    User user(userid,"","","offline");
    userModel.updateState(user);
    
}
// 处理客户端异常退出
void ChatService::clientCloseException(const TcpConnectionPtr &conn)
{
    User user;
    {
        std::lock_guard<std::mutex> lock(connMutex_);
        for(auto it = userConnMap_.begin(); it != userConnMap_.end(); it++)
        {
            if(it->second == conn)
            {
                // 从map表删除用户的连接信息
                userConnMap_.erase(it);
                user.setId(it->first);
                break;
            }
        }
    }
    //用户注销，相当于下线，在redis中取消订阅通道
    redis_.unsubscribe(user.getId());

    //更新用户的状态信息
    if(user.getId() != -1)
    {
         user.setState("offline");
         userModel.updateState(user);
    }
}
// 服务器异常，业务重置方法
void ChatService::reset()
{
    //把online状态的用户，全部设置为offline
    userModel.resetState();
}


void ChatService::handleRedisSubscibeMessage(int channel, std::string message)
{
    lock_guard<std::mutex> lock(connMutex_);
    auto it = userConnMap_.find(channel);
    if(it != userConnMap_.end())
    {
        it->second->send(message);
        return;
    }
    offlineMsgModel.insert(channel, message);
}