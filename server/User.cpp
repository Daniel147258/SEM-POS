#include "User.h"

User::User(const std::string& name, const std::string& password) {
    this->name = name;
    this->password = password;
}

std::string User::getHeslo() { //mutexovanie
    std::unique_lock<std::mutex> lock(passwordMutex);
    lock.unlock();
    return password;
}

std::string User::getMeno() { //mutexovanie
    std::unique_lock<std::mutex> lock(nameMutex);
    lock.unlock();
    return name;
}