#include <iostream>
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

    TableColumnBase(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : name(columnName), notNull(isNotNull), primaryKey(isPrimaryKey) {}

    virtual ~TableColumnBase() {}

    std::string getName() const {
        return name;
    }

    bool isPrimaryKey(){
        return primaryKey;
    }

    bool isNotNullColumn(){
        return notNull;
    }

    virtual void printHeader() const = 0;
    virtual void printValue(size_t rowIndex) const = 0;
    virtual void addValue(const std::string& value) = 0;
    virtual void uploadValue(const std::string& value){

    }
};


class IntColumn : public TableColumnBase {
private:
    std::vector<std::optional<int>> values;

public:
    IntColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << getName() << " (int)\t";
    }

    void printValue(size_t rowIndex) const override {
        if (values[rowIndex].has_value()) {
            std::cout << values[rowIndex].value() << "\t\t";
        } else {
            std::cout << "NULL\t\t";
        }
    }

    void addValue(const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            values.push_back(std::nullopt);
        } else{
            values.push_back(std::stoi(value));
        }
    }
};


class DoubleColumn : public TableColumnBase {
private:
    std::vector<std::optional<double>> values;
public:
    DoubleColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << getName() << " (double)\t";
    }

    void printValue(size_t rowIndex) const override {
        if (values[rowIndex].has_value()) {
            std::cout << values[rowIndex].value() << "\t\t";
        } else {
            std::cout << "NULL\t\t";
        }
    }

    void addValue(const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            values.push_back(std::nullopt);
        } else{
            values.push_back(std::stod(value));
        }
    }
};


class StringColumn : public TableColumnBase {
private:
    std::vector<std::string> values;

public:
    StringColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << getName() << " (string)\t";
    }

    void printValue(size_t rowIndex) const override {
        std::cout << values[rowIndex] << "\t\t";
    }

    void addValue(const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            std::string value1 = "NULL";
            values.push_back(value1);
        } else{
            values.push_back(value);
        }
    }
};


class BoolColumn : public TableColumnBase {
private:
    std::vector<std::optional<bool>> values;

public:
    BoolColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << getName() << " (bool)\t";
    }

    void printValue(size_t rowIndex) const override {
        if (values[rowIndex].has_value()) {
            std::cout << (values[rowIndex] ? "true" : "false") << "\t\t";
        } else {
            std::cout << "NULL\t\t";
        }
    }

    void addValue(const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !isNotNullColumn()) || value == "NULL"){
            values.push_back(std::nullopt);
        } else{
            values.push_back(value == "true" ? true : false);
        }
    }
};

class DateColumn : public TableColumnBase {
public:
    std::vector<std::string> values;

    DateColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << getName() << " (date)\t";
    }

    void printValue(size_t rowIndex) const override {
        std::cout << values[rowIndex] << "\t\t";
    }

    void addValue(const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            std::string value1 = "NULL";
            values.push_back(value1);
        } else{
            values.push_back(value);
        }
    }
};

class Table {
private:
    std::string tableName;
    std::vector<TableColumnBase*> columns;

public:
    Table(const std::string& name) : tableName(name) {}

    ~Table() {

        for (auto column : columns) {
            delete column;
        }
    }

    void addColumn(TableColumnBase* column) {
        columns.push_back(column);
    }

    void addColumnValue(size_t columnIndex, const std::string& value) {
        if (columnIndex < columns.size()) {
            columns[columnIndex]->addValue(value);
        } else {
            std::cerr << "Invalid column index." << std::endl;
        }
    }

    void addRow(const std::vector<std::string>& row) {
        if (row.size() > columns.size()) {
            std::cerr << "Too many elements in the row. Ignoring extra values." << std::endl;
        }

        for (size_t i = 0; i < columns.size(); ++i) {
            if(i >= row.size()){
                addColumnValue(i, "");
            }
            else {
                addColumnValue(i, row[i]);
            }
        }
    }


    void printHeader() const {
        for (const auto& column : columns) {
            column->printHeader();
        }
        std::cout << std::endl;
    }


    void printRow(size_t rowIndex) const {
        for (const auto& column : columns) {
            column->printValue(rowIndex);
        }
        std::cout << std::endl;
    }

    void pritnAllRows(){
        for (int i = 0; i < columns.size() - 1 ; ++i) {
            this->printRow(i);
        }
    }
};

//int main() {
//    Table myTable("SampleTable");
//
//
//    myTable.addColumn(new IntColumn("ID", true, true));
//    myTable.addColumn(new StringColumn("Name", true, false));
//    myTable.addColumn(new DoubleColumn("Salary", false, false));
//    myTable.addColumn(new BoolColumn("Active", false, false));
//    myTable.addColumn(new DateColumn("Date", false, false));
//
//
//    myTable.addRow({"1", "John", "50000.0", "true"});
//    myTable.addRow({"2", "Lopata", "60000.1", "false"});
//    myTable.addRow({"3", "Doe","25.4",""});
//    myTable.addRow({"4","Dan","","","25.4.2023"});
//    myTable.printHeader();
//
//    myTable.pritnAllRows();
//
//
//    return 0;
//}
