#ifndef TABLE_H
#define TABLE_H

#include <iostream>
#include <vector>
#include "./TableColumnBase.h"
#include "../User.h"


class Table {
private:
    std::string tableName;
    std::vector<TableColumnBase*> columns;
    std::vector<User*> selecting;
    std::vector<User*> updating;
    std::vector<User*> adding;
    std::vector<User*> deleting;
    int countRows;
    User* creator;
    std::mutex getListOfUserRightsMtx;
    std::mutex getCreatorMtx;

public:
    Table(const std::string& name, User* creator);
    ~Table();

    void addColumn(TableColumnBase* column);
    bool containsPK(size_t columnIndex, const std::string& value);
    void addColumnValue(size_t columnIndex, const std::string& value);
    void addRow(const std::vector<std::string>& row);
    void printHeader() const;
    void printIndexingColumns() const;
    void printRow(size_t rowIndex) const;
    void pritnAllRows();
    void uploadColumnValue(size_t columnIndex, size_t rowIndex, const std::string& value);
    int uploadAllColumnValues(size_t columnIndex, const std::string& oldValue, const std::string& newValue);
    void deleteRow(size_t rowIndex);
    int deleteRowsByValue(size_t columnIndex, const std::string& value);
    int getCountRows();
    std::string getName();
    User* getCreator();
    std::string getRow(int index);
    std::string getAllRows();
    bool existsColumnIndex(int index);
    std::string getColumnsIndexes();
    int getNumberOfColumns();
    std::string getColumnDescription(int index);
    bool isColumnNullAble(int index);
    std::string getColumnType(int index);
    bool existsInTablePrimaryKey();
    std::string getHeader();
    bool isColumnPrimaryKer(int index);
    bool canUserSelect(User* client);
    bool canUserUpdate(User* client);
    bool canUserAdd(User* client);
    bool canUserDelete(User* client);
    std::string addRightSelect(User* client);
    std::string addRightUpdate(User* client);
    std::string addRightAdd(User* client);
    std::string addRightDelete(User* client);
    std::string getListOfUserRights(User* client);
};

#endif // TABLE_H
