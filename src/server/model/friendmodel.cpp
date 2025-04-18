#include "friendmodel.hpp"
//#include "db.h"
#include "Connection.h"
#include "dbConnnectionPool.h"
//添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    char sql[1024] = {0};
    sprintf(sql,"insert into friend values(%d,%d)",userid,friendid);
    shared_ptr<Connection> conn = pool->getConnection();
    conn->update(sql);
}

//返回用户好友列表
std::vector<User> FriendModel::query(int userid)
{
    std::vector<User> vec;
    char sql[1024] = {0};
    sprintf(sql,"select a.id,a.name,a.state from user a inner join friend b on a.id = b.friendid where b.userid=%d",userid);
    shared_ptr<Connection> conn = pool->getConnection();
    if(conn->getConnectState())
    {
        MYSQL_RES *res = conn->query(sql);
        if(res != nullptr)
        {
            MYSQL_ROW row;
            while((row = mysql_fetch_row(res)) != nullptr)
            {
                User user;
                user.setId(atoi(row[0]));
                user.setName(row[1]);
                user.setState(row[2]);
                vec.push_back(user);
            }
            mysql_free_result(res);
        }
    }
    return vec;
}