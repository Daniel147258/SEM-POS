#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include "./Table/Table.h"
#include "./User.h"
#include <fstream>
#include <sstream>

class Database {
private:
    std::vector<Table*> tables;
    std::vector<User*> users;
    std::mutex addTableMtx;
    std::mutex existTableMtx;
    std::mutex existsUser;
    std::mutex addUsera;
    std::mutex getUserMtx;
    std::mutex getTablesMtx;
    std::mutex getTableMtx;
    std::mutex deleteTableMtx;
    std::mutex getUserTablesMtx;
    std::mutex getUserTablesWithRightsMtx;
    void removeTableByName(const std::string& name);

public:
    Database();
     ~Database();
    void addTable(Table* table);
    bool existTable(const std::string& name);
    void addUser(const std::string& name, const std::string& password);
    bool existUsername(const std::string& name);
    User* getUser(const std::string& name);
    std::string getTables();
    Table* getTable(const std::string &name);

    //bez mutexu
    std::string getRowByIndex(const std::string& name, int indexOfRow);
    std::string getAllRows(const std::string &nameTable);
    bool existsColumnIndex(const std::string &nameTable, int index);
    std::string getColumnsIndexes(const std::string &tableName);
    int getNumberOfColumns(const std::string &tableName);
    std::string getColumnDescription(const std::string &tableName, int index);
    bool isColumnNullAble(const std::string &tableName, int index);
    std::string getTypeOfColumn(const std::string &tableName, int index);
    int DeleteRow(const std::string &tableName, int index, const std::string &value);
    void addRow(const std::string &name, std::vector<std::string> &row);
    bool isTableUser(const std::string& tableName, User* client);
    void writeData();
    void loadData();
    // koniec kde nie je potrebny mutex

    std::string deleteTable(const std::string& name, User* client);
    std::string getUserTables(User* client);
    std::string getUserTablesWithRights(User* client);

};

#endif // DATABASE_H
