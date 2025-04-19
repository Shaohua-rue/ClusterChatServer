#pragma once 
#include <string>
#include <vector>
#include "Connection.h"
#include "dbConnnectionPool.h"
class OfflineMsgModel
{
public:
    OfflineMsgModel():pool (ConnectionPool::getConnectionPool()){}
    // 存储用户的离线消息
    void insert(int userid, string msg);

    // 查询用户的离线消息
    vector<string> query(int userid);

    // 移除用户的离线消息
    void remove(int userid);
private:
    ConnectionPool* pool;
};