#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include "./Table/Table.h"
#include "./User.h"

class Database {
private:
    std::vector<Table*> tables;
    std::vector<User*> users;
    std::mutex addTableMtx;
    std::mutex existsUser;
    std::mutex addUsera;
    std::mutex getUserMtx;
    std::mutex getTablesMtx;
    std::mutex getTableMtx;
public:
    void addTable(Table* table);
    bool existTable(const std::string& name) const;
    void addUser(const std::string& name, const std::string& password);
    bool existUsername(const std::string& name);
    User* getUser(const std::string& name);
    std::string getTables();
    Table* getTable(const std::string &name);
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
};

#endif // DATABASE_H
