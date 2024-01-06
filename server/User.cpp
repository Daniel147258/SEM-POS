#include "User.h"

User::User(const std::string& name, const std::string& password) {
    this->name = name;
    this->password = password;
}

std::string User::getHeslo() { //mutexovanie
    return password;
}

std::string User::getMeno() { //mutexovanie
    return name;
}