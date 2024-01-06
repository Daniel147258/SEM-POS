#ifndef USER_H
#define USER_H
#include <iostream>
#include <string>


class User{
private:
    std::string name;
    std::string password;


public:
    User(const std::string& name, const std::string& password);
    std::string getHeslo();
    std::string getMeno();
};
#endif // USER_H