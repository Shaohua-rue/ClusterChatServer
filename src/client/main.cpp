#include "client.hpp"

//--------------------聊天客户端实现，main线程用做发送线程，子线程用做接收线程--------------------
int main(int argc, char* argv[])
{
    char* ip;
    uint16_t port;
    if(argc < 2)
    {
       ip = "192.168.6.129";
       port = 1200;
    }
    else
    {
        ip = "192.168.6.129";
        port = atoi(argv[1]);
    }
    //创建client端的socket
    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1)
    {
        cerr << "socket create error" << endl;
        exit(-1);
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if(connect(clientfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        cerr << "connect error" << endl;
        exit(-1);
    }

    // 初始化读写线程通信用的信号量
    sem_init(&rwsem, 0, 0);

    // 连接服务器成功，启动接收子线程
    std::thread readTask(readTaskHandler, clientfd); // pthread_create
    readTask.detach();                               // pthread_detach
    for(;;)
    {
         // 显示首页面菜单 登录、注册、退出
        cout << "========================" << endl;
        cout << "1. login" << endl;
        cout << "2. register" << endl;
        cout << "3. quit" << endl;
        cout << "========================" << endl;
        cout << "choice:";
        int choice = 0;
        cin >> choice;
        cin.get(); // 读掉缓冲区残留的回车

        switch(choice)
        {
            /******************************登录***************************************/ 
            case 1: 
                {
                    int id = 0;
                    char pwd[50] = {0};
                    cout << "userid:";
                    cin >> id;
                    cin.get();
                    cout << "userpwd:";
                    cin.getline(pwd, 50);

                    json js;
                    js["msgid"] = LOGIN_MSG;
                    js["id"] = id;
                    js["password"] = pwd;
                    string sendbuf = js.dump();
                    g_isLoginSuccess = false;

                    if(send(clientfd, sendbuf.c_str(), sendbuf.size(), 0) == -1)
                    {
                        cerr << "send login msg error:" << endl;
                        close(clientfd);
                        exit(-1);
                    }
                    sem_wait(&rwsem);   // 等待信号量，由子线程处理完登录的响应消息后，通知这里
                    if(g_isLoginSuccess)
                    {
                        // 登录成功，则进入聊天页面
                        isMainMenuRunning = true;
                        mainMenu(clientfd);
                    }
                }
                break;
            /*************************************注册*************************************/ 
            case 2:    
                {
                    char name[50] = {0};
                    char pwd[50] = {0};

                    cout << "name:";
                    cin.getline(name, 50);
                    cout << "password:";
                    cin.getline(pwd, 50);

                    json js;
                    js["msgid"] = REG_MSG;
                    js["name"] = name;
                    js["password"] = pwd;

                    string sendbuf = js.dump();

                    int len = send(clientfd, sendbuf.c_str(), sendbuf.size(), 0);
                    if(len == -1)
                    {
                        cerr << "send reg msg error:" << endl;
                        close(clientfd);
                        exit(-1);
                    }
                    sem_wait(&rwsem);//等待信号量，子线程处理完注册会通知
                 

                }
                break;
            case 3:
                close(clientfd);
                sem_destroy(&rwsem);
                exit(0);
            default:
                cerr << "invalid input" << endl;
            break;
        }
    }
   

    return 0;
}
