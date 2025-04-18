#include "usemodel.hpp"
//#include "db.h"
#include <iostream>
#include "Logger.h"
// 注册新用户
bool UserModel::insert(User &user)
{
    //1.组装sql语句
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "insert into user(name, password, state) values('%s', '%s', '%s')",
             user.getName().c_str(), user.getPwd().c_str(), user.getState().c_str());
             
    shared_ptr<Connection> conn = pool->getConnection();
    if(conn->getConnectState())
    {
        if (conn->update(sql))
        {
            //获取插入成功的用户数据生成的主键
            user.setId(mysql_insert_id(conn->getConnection()));
            return true;
        }
    }
    return false;

}

// 根据用户号码查询用户信息
User UserModel::query(int id)
{
    //1.组装sql语句
    char sql[1024] = {0};
    snprintf(sql, sizeof(sql), "select * from user where id = %d", id);
    shared_ptr<Connection> conn = pool->getConnection();
    if(conn->getConnectState())
    {
        MYSQL_RES* res= conn->query(sql);
        if (res != nullptr)
        {
            MYSQL_ROW row = mysql_fetch_row(res);   //获取一行数据
            if (row != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setPwd(row[2]);
                user.setState(row[3]);
                mysql_free_result(res);
                return user;
            }
        }

    }
    return User();
}

// 更新用户状态信息
bool UserModel::updateState(User user)
{
     //1.组装sql语句
     char sql[1024] = {0};
     sprintf(sql, "update user set state = '%s' where id = %d", user.getState().c_str(), user.getId());
 
     shared_ptr<Connection> conn = pool->getConnection();
     if(conn->getConnectState())
     {
         if (conn->update(sql))
         {
             return true;
         }
     }
     return false;
}

// 重置用户的状态信息
void UserModel::resetState()
{
    //1.组装sql语句
    char sql[1024] = "update user set state = 'offline' where state = 'online'";

    shared_ptr<Connection> conn = pool->getConnection();
    if(conn->getConnectState())
    {
        conn->update(sql);
    }
}