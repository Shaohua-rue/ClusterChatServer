#pragma once
#include <vector>
#include "user.hpp"
#include "Connection.h"
#include "dbConnnectionPool.h"
//维护好友信息的接口方法
class FriendModel
{
public:
    FriendModel():pool (ConnectionPool::getConnectionPool()){}
    //添加好友关系
    void insert(int userid, int friendid);

    //返回用户好友列表
    std::vector<User> query(int userid);
private:
    ConnectionPool* pool;

};