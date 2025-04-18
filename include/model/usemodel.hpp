#pragma once
#include "user.hpp"
//user表的数据操作类
class UserModel {
public:
    // 注册新用户
    bool insert(User &user);

    // 根据用户号码查询用户信息
    User query(int id);

    // 更新用户状态信息
    bool updateState(User user);

    // 重置用户的状态信息
    void resetState();
};