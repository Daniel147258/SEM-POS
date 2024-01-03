
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>
#include <chrono>

class TableColumnBase {
private:
    std::string name;
    bool notNull;
    bool primaryKey;

public:
    TableColumnBase(const std::string& columnName, bool isNotNull, bool isPrimaryKey);
    virtual ~TableColumnBase();

    std::string getName() const;
    bool isPrimaryKey();
    bool isNotNullColumn();

    virtual void printHeader() const = 0;
    virtual void printValue(std::size_t rowIndex) const = 0;
    virtual void addValue(const std::string& value) = 0;
    virtual void uploadValue(std::size_t rowIndex, const std::string& value) = 0;
    virtual void uploadAllValues(const std::string& oldValue, const std::string& newValue) = 0;
    virtual int getSize() = 0;
    virtual bool containsValue(const std::string& value) = 0;
    virtual void deleteValue(std::size_t rowIndex) = 0;
    virtual std::string getValue(std::size_t rowIndex) const = 0;
};

class IntColumn : public TableColumnBase {
    // ... (bez implementácie)
};

class DoubleColumn : public TableColumnBase {
    // ... (bez implementácie)
};

class StringColumn : public TableColumnBase {
    // ... (bez implementácie)
};

class BoolColumn : public TableColumnBase {
    // ... (bez implementácie)
};

class DateColumn : public TableColumnBase {
    // ... (bez implementácie)
};

class Table {
private:
    std::string tableName;
    std::vector<TableColumnBase*> columns;
    int countRows;

public:
    Table(const std::string& name);
    ~Table();

    void addColumn(TableColumnBase* column);
    bool containsPK(size_t columnIndex,const std::string& value);
    void addColumnValue(size_t columnIndex, const std::string& value);
    void addRow(const std::vector<std::string>& row);
    void printHeader() const;
    void printIndexingColumns() const;
    void printRow(size_t rowIndex) const;
    void pritnAllRows();
    void uploadColumnValue(size_t columnIndex, size_t rowIndex, const std::string& value);
    void uploadAllColumnValues(size_t columnIndex, const std::string& oldValue, const std::string& newValue);
    void deleteRow(size_t rowIndex);
    void deleteRowsByValue(size_t columnIndex, const std::string& value);
    int getCountRows();
    std::string getName();
};

