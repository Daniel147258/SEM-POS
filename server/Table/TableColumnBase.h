#ifndef TABLECOLUMNBASE_H
#define TABLECOLUMNBASE_H

#include <string>
#include <algorithm>
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
    virtual void printValue(size_t rowIndex) const = 0;
    virtual void addValue(const std::string& value) = 0;
    virtual void uploadValue(size_t rowIndex, const std::string& value) = 0;
    virtual int uploadAllValues(const std::string& oldValue, const std::string& newValue) = 0;
    virtual int getSize() = 0;
    virtual bool containsValue(const std::string& value) = 0;
    virtual int deleteValue(const std::string& value) = 0;
    virtual std::string getValue(int rowIndex) = 0;
    virtual std::string getDescription() = 0;
    virtual std::string getTypea() = 0;
};

#endif // TABLECOLUMNBASE_H
