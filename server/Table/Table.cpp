#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>

// Base class for TableColumn
class TableColumnBase {
public:
    std::string name;
    bool notNull;
    bool primaryKey;

    TableColumnBase(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : name(columnName), notNull(isNotNull), primaryKey(isPrimaryKey) {}

    virtual ~TableColumnBase() {}

    virtual void printHeader() const = 0;
    virtual void printValue(size_t rowIndex) const = 0;
    virtual void addValue(const std::string& value) = 0;
};

// TableColumn for int
class IntColumn : public TableColumnBase {
public:
    std::vector<std::optional<int>> values;

    IntColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << name << " (int)\t";
    }

    void printValue(size_t rowIndex) const override {
        if (values[rowIndex].has_value()) {
            std::cout << values[rowIndex].value() << "\t\t";
        } else {
            std::cout << "NULL\t\t";
        }
    }

    void addValue(const std::string& value) override {
        if (value.empty() && notNull) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !notNull) || value.size() <= 0){
            values.push_back(std::nullopt);
        } else{
            values.push_back(std::stoi(value));
        }
    }
};

// TableColumn for double
class DoubleColumn : public TableColumnBase {
public:
    std::vector<std::optional<double>> values;

    DoubleColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << name << " (double)\t";
    }

    void printValue(size_t rowIndex) const override {
        if (values[rowIndex].has_value()) {
            std::cout << values[rowIndex].value() << "\t\t";
        } else {
            std::cout << "NULL\t\t";
        }
    }

    void addValue(const std::string& value) override {
        if (value.empty() && notNull) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !notNull) || value.size() <= 0){
            values.push_back(std::nullopt);
        } else{
            values.push_back(std::stoi(value));
        }
    }
};

// TableColumn for string
class StringColumn : public TableColumnBase {
public:
    std::vector<std::string> values;

    StringColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << name << " (string)\t";
    }

    void printValue(size_t rowIndex) const override {
        std::cout << values[rowIndex] << "\t\t";
    }

    void addValue(const std::string& value) override {
        if (value.empty() && notNull) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !notNull) || value.size() <= 0){
            std::string value1 = "NULL";
            values.push_back(value1);
        } else{
            values.push_back(value);
        }
    }
};

// TableColumn for bool
class BoolColumn : public TableColumnBase {
public:
    std::vector<std::optional<bool>> values;

    BoolColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
            : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

    void printHeader() const override {
        std::cout << name << " (bool)\t";
    }

    void printValue(size_t rowIndex) const override {
        if (values[rowIndex].has_value()) {
            std::cout << (values[rowIndex] ? "true" : "false") << "\t\t";
        } else {
            std::cout << "NULL\t\t";
        }
    }

    void addValue(const std::string& value) override {
        if (value.empty() && notNull) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }
        if((value.empty() && !notNull) || value == "NULL"){
            values.push_back(std::nullopt);
        } else{
            values.push_back(value == "true" ? true : false);
        }
    }
};

// Table class containing a vector of TableColumnBase
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
};

int main() {
    Table myTable("SampleTable");

    // Add columns of different types with attributes
    myTable.addColumn(new IntColumn("ID", true, true));
    myTable.addColumn(new StringColumn("Name", true, false));
    myTable.addColumn(new DoubleColumn("Salary", false, false));
    myTable.addColumn(new BoolColumn("Active", false, false));

    // Add rows with different types
    myTable.addRow({"1", "John", "50000.0", "true"});
    myTable.addRow({"2", "Lopata", "60000.0", "false"});  // This row will trigger a NOT NULL error
    myTable.addRow({"3", "Doe"});    // This row will trigger a NOT NULL error

    // Print table header
    myTable.printHeader();

    // Print rows
    for (size_t i = 0; i < 3; ++i) {
        myTable.printRow(i);
    }

    return 0;
}
