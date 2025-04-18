#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <string>
#include <thread>

//---网络通信相关
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <semaphore.h>
#include <atomic>

#include "json.hpp"

#include "user.hpp"
#include "group.hpp"
#include "public.hpp"

using namespace std;
using json = nlohmann::json;

extern User g_CurrentUser;             //记录当前系统的用户信息
extern vector<User> g_UsersFriendList; //记录当前用户的好友列表
extern vector<Group> g_GroupsList;     //记录当前用户的群组列表   

extern sem_t rwsem;        // 用于读写线程之间的通信
extern atomic_bool g_isLoginSuccess;// 记录登录状态
extern bool isMainMenuRunning ; // 控制主菜单页面程序

//-------函数声明
void mainMenu(int);                      //主菜单
void showCurrentUserData();             //显示当前登录成功用户的基本信息
void readTaskHandler(int clientfd);     //接收线程
string getCurrentTime();                   ///获取当前时间


void doLoginResponse(json &response);   //处理登录响应
void doRegResponse(json &response);     //处理注册响应



// "help" command handler
void help(int fd = 0, string str = "");
// "chat" command handler
void chat(int, string);
// "addfriend" command handler
void addfriend(int, string);
// "creategroup" command handler
void creategroup(int, string);
// "addgroup" command handler
void addgroup(int, string);
// "groupchat" command handler
void groupchat(int, string);
// "loginout" command handler
void loginout(int, string);

