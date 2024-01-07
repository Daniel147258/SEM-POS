#include "database.h"
#include "thread"
#include <algorithm>

void Database::addTable(Table* table) {
    if (!existTable(table->getName())) {
        std::unique_lock<std::mutex> lock(addTableMtx);
        tables.push_back(table);
        lock.unlock();
    }else{
        std::cerr << "Error with add table to database!!\n";
    }
}

bool Database::existTable(const std::string& name) const { //mutex chyba
    for (auto table : tables) {
        if (table->getName() == name) {
            return true;
        }
    }
    return false;
}

void Database::addUser(const std::string& name, const std::string& password) {
    std::unique_lock<std::mutex> lock(existsUser);
    if (!existUsername(name)) {
        users.push_back(new User(name, password));
    }
    lock.unlock();
}

bool Database::existUsername(const std::string& name) {
    std::unique_lock<std::mutex> lock(addUsera);
    for (auto usera : users) {
        if (usera->getMeno() == name) {
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

User* Database::getUser(const std::string& name) {
    std::unique_lock<std::mutex> lock(getUserMtx);
    for (auto usera : users) {
        if (usera->getMeno() == name) {
            lock.unlock();
            return usera;
        }
    }
    lock.unlock();
    return nullptr;
}

std::string Database::getTables(){
    std::unique_lock<std::mutex> lock(getTablesMtx);
    std::string a = "Not tabels yet";
    int i = 0;
    if(!tables.empty()) {
        a = "";
        for (auto tab: tables) {
            if (i % 10 == 0) {
                a += tab->getName() + ",\n";
            } else {
                a += tab->getName() + ", ";
            }
            ++i;
        }
    }
    lock.unlock();
    return a;
}

Table* Database::getTable(const std::string& name){
    std::unique_lock<std::mutex> lock(getTableMtx);
    for(auto tab : tables){
        if(tab->getName() == name){
            lock.unlock();
            return tab;
        }
    }
    lock.unlock();
    return nullptr;
}

std::string Database::getRowByIndex(const std::string& name, int indexOfRow){ //chyba mutex
    return getTable(name)->getRow(indexOfRow);
}

std::string Database::getAllRows(const std::string& nameTable){ //chyba mutex
    return getTable(nameTable)->getAllRows();
}

bool Database::existsColumnIndex(const std::string& nameTable, int index){ //chyba mutex
    return getTable(nameTable)->existsColumnIndex(index);
}

std::string Database::getColumnsIndexes(const std::string& tableName){ //chyba mutex
    return getTable(tableName)->getColumnsIndexes();
}

int Database::getNumberOfColumns(const std::string& tableName){ // chyba mutex
    return getTable(tableName)->getNumberOfColumns();
}

std::string Database::getColumnDescription(const std::string& tableName, int index){ //chyba mutex
    return getTable(tableName)->getColumnDescription(index);
}

bool Database::isColumnNullAble(const std::string& tableName, int index){ //chyba mutex
    return getTable(tableName)->isColumnNullAble(index);
}

std::string Database::getTypeOfColumn(const std::string& tableName, int index){ //chyba mutex
    return getTable(tableName)->getColumnType(index);
}

int Database::DeleteRow(const std::string& tableName, int index, const std::string& value){ //chyba mutex
    return getTable(tableName)->deleteRowsByValue(index, value);
}

void Database::addRow(const std::string& name, std::vector<std::string>& row){
    getTable(name)->addRow(row);
}

std::string Database::deleteTable(const std::string& name, User* client) {
    std::unique_lock<std::mutex> lock(deleteTableMtx);
    std::string a = "Table doesnt exists !!";
    Table* table = getTable(name);
    if(table){
        if(table->getCreator()->getMeno() == client->getMeno()){
            a = "Table successfully removed";
            removeTableByName(name);
            delete table;
        } else{
            a = "You don't have the right to delete the table !!";
        }
    }
    lock.unlock();
    return a;
}

void Database::removeTableByName(const std::string& name) {
    auto it = std::remove_if(tables.begin(), tables.end(),
                             [name](Table* table) {
                                 return name == table->getName();
                             });

    tables.erase(it, tables.end());
}

Database::~Database() {
    for(auto tab : tables){
        if(tab != nullptr){
            delete tab;
        }
    }
    users.clear();
}

std::string Database::getUserTables(User *client) {
    std::string a = "You dont have any Table\n";
    bool first = false;
    for(auto tab: tables){
        if(tab->getCreator()->getMeno() == client->getMeno()){
            if(!first){
                first = true;
                a = "Your Tables:\n" + tab->getName() + ", ";
            }
            else{
                a += tab->getName() + ", ";
            }
        }
    }
    return a;
}

std::string Database::getUserTablesWithRights(User *client) {
    std::unique_lock<std::mutex> lock(getUserTablesWithRightsMtx);
    std::string a = "Tables with your rights:\n";
    int i = 1;
    if(tables.size() == 0){
        a += "You dont have any right to table\n";
        lock.unlock();
        return a;
    }
    else{
        for(auto tab : tables) {
            a += std::to_string(i) + ". "+tab->getListOfUserRights(client);
            i++;
        }
    }
    lock.unlock();
    return a;
}

