#pragma once 
#include "db.h"
#include <string>
#include <vector>
class OfflineMsgModel
{
public:
    // 存储用户的离线消息
    void insert(int userid, string msg);

    // 查询用户的离线消息
    vector<string> query(int userid);

    // 移除用户的离线消息
    void remove(int userid);
};