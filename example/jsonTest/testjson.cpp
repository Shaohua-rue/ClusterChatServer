#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "json.hpp"

using json = nlohmann::json;

//普通数据的序列化
void function1()
{
    json js;
    //添加数组
    js["id"] = {1, 2, 3, 4, 5};

    //添加key-value
    js["name"] = "zhangsan";

    //添加对象
    js["msg"]["one"] =1;
    js["msg"]["two"] =2;
    js["msg"]["three"] =3;

    //上面等同于下面这一次性添加数组对象，且js是去重的，重复插入只保留一次结果
    js["msg"] = {
        {"one", 1},
        {"two", 2},
        {"three", 3},
    };
    std::cout << js << std::endl;
}

//容器的序列化
void function2()
{
    std::vector<int> vec{1, 2, 3, 4, 5};
    json js;
    js["vec"] = vec;
    std::cout << js << std::endl;

    std::map<std::string, int> map;
    map.insert(std::pair<std::string, int>("zhangsan", 100));
    map.insert(std::pair<std::string, int>("lisi", 90));
    map.insert(std::pair<std::string, int>("wangwu", 80));
    json js2;
    js2["map"] = map;
    std::cout << js2 << std::endl;
}

std::string function3()
{
    json js;
    //添加数组
    js["id"] = {1, 2, 3, 4, 5};

    //添加key-value
    js["name"] = "zhangsan";

    //添加对象
    js["msg"]["one"] =1;
    js["msg"]["two"] =2;
    js["msg"]["three"] =3;

    //上面等同于下面这一次性添加数组对象，且js是去重的，重复插入只保留一次结果
    js["msg"] = {
        {"one", 1},
        {"two", 2},
        {"three", 3},
    };
    return js.dump();
}

std::string function4()
{
    std::vector<int> vec{1, 2, 3, 4, 5};
    json js;
    js["vec"] = vec;
    std::cout << js << std::endl;

    std::map<std::string, int> map;
    map.insert(std::pair<std::string, int>("zhangsan", 100));
    map.insert(std::pair<std::string, int>("lisi", 90));
    map.insert(std::pair<std::string, int>("wangwu", 80));
    json js2;
    js2["map"] = map;
    return js2.dump();      //json------>string
}
int main()
{
    //----json 序列化 ----
    //function1();
    //function2();

    //----json 反序列化 ----
    std::string recvBuf = function3();
    json jsbuf = json::parse(recvBuf);          //string ------>json
    std::cout << jsbuf["id"] << std::endl;
    std::cout << jsbuf["msg"]["one"] << std::endl;

    std::string str = function4();
    json js = json::parse(str);
    auto map = js["map"];
    std::cout << map["zhangsan"] << std::endl;
    //json ====>STL
    std::vector<int> vec = jsbuf["id"];
    for (auto it = vec.begin(); it != vec.end(); it++)
    {
        std::cout << *it << std::endl;
    }
    std::map<std::string, int> map1 = js["map"];
    for (auto it = map1.begin(); it != map1.end(); it++)
    {
        std::cout << it->first << ":" << it->second << std::endl;
    }
    return 0;
}