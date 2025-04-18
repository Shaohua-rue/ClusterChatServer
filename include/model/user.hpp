#pragma once

#include <string>

//User表的ORM类
class User
{
public:
    User(int id = -1, std::string name = "", std::string pwd = "", std::string state = "offline")
    {
        this->id = id;
        this->name = name;
        this->password = pwd;
        this->state = state;
    }
    void setId(int id) {this->id = id;}
    void setName(std::string name) {this->name = name;}
    void setPwd(std::string pwd){this -> password = pwd;}
    void setState(std::string state){this->state = state;}
    int getId()const {return this->id;}
    std::string getName()const {return this->name;}
    std::string getPwd() const {return this->password;}

    std::string getState() const {return this->state;}


protected:
    int id;
    std::string name;
    std::string password;
    std::string state;
};