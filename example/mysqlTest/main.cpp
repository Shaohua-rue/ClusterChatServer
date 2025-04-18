#include <iostream>
#include <mysql/mysql.h>
int main()
{
    MYSQL *conn;
    conn = mysql_init(nullptr);
    if (!mysql_real_connect(conn, "127.0.0.1", "shaohua", "010407", "chat", 3306, nullptr, 0))
    {
        std::cout<<"mysql connect error"<<std::endl;
    }
    else {
        std::cout<<"mysql connect success"<<std::endl;
    }
    std::cout<<"hello world"<<std::endl;
    std::cout<<"hello world"<<std::endl;
    return 0;
}