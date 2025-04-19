#pragma once
#include "group.hpp"
#include <string>
#include <vector>
#include "Connection.h"
#include "dbConnnectionPool.h"
//维护群组信息的操作接口方法
class GroupModel
{
public:
    GroupModel():pool (ConnectionPool::getConnectionPool()){}
    //创建群组
    bool createGroup(Group &group);

    //加入群组
    void addGroup(int userid,int groupid, std::string role);

    //查询用户所在群组信息
    std::vector<Group> queryGroups(int userid);

    //根据指定的groupid查询群组用户的id列表，除了userid自己，主要用户群聊业务给群组其他成员群发信息
    std::vector<int> queryGroupUsers(int userid,int groupid);
private:
    ConnectionPool* pool;
};