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
            : name(columnName), notNull(isNotNull), primaryKey(isPrimaryKey) {

    }

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
    virtual void uploadValue(size_t rowIndex, const std::string& value) = 0;
    virtual void uploadAllValues(const std::string& oldValue, const std::string& newValue) = 0;
    virtual int getSize() = 0;
    virtual bool containsValue(const std::string& value) = 0;
    virtual void deleteValue(size_t rowIndex) = 0;
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

    void uploadValue(size_t rowIndex, const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }

        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            values[rowIndex] = std::nullopt;
        } else{
            values[rowIndex] = std::stoi(value);
        }
    }
    void uploadAllValues(const std::string& oldValue, const std::string& newValue) override{
        if (oldValue.empty()) {
            throw std::invalid_argument("Old value cannot be empty.");
        }

        int newIntValue = std::stoi(newValue);

        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i].has_value() && values[i].value() == std::stoi(oldValue)) {
                values[i] = newIntValue;
            }
        }
    }

    int getSize() override{
        return values.size();
    }

    bool containsValue(const std::string& value) override{
        int number = std::stoi(value);
        for(const auto val : values ){
            if(number == val){
                return true;
                break;
            }
        }
        return false;
    }

    void deleteValue(size_t rowIndex) override {
        if (rowIndex < values.size()) {
            std::vector<std::optional<int>> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (i != rowIndex) {
                    tempValues.push_back(values[i]);
                }
            }
            values.clear();
            values = tempValues;
            tempValues.clear();
        } else {
            std::cerr << "Invalid row index." << std::endl;
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
    void uploadValue(size_t rowIndex, const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }

        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            values[rowIndex] = std::nullopt;
        } else{
            values[rowIndex] = std::stod(value);
        }
    }
    void uploadAllValues(const std::string& oldValue, const std::string& newValue) override{
        if (oldValue.empty()) {
            throw std::invalid_argument("Old value cannot be empty.");
        }

        int newValue2 = std::stod(newValue);

        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i].has_value() && values[i].value() == std::stod(oldValue)) {
                values[i] = newValue2;
            }
        }
    }
    int getSize() override{
        return values.size();
    }

    bool containsValue(const std::string& value) override{
        int number = std::stod(value);
        for(const auto val : values ){
            if(number == val){
                return true;
                break;
            }
        }
        return false;
    }

    void deleteValue(size_t rowIndex) override {
        if (rowIndex < values.size()) {
            std::vector<std::optional<double>> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (i != rowIndex) {
                    tempValues.push_back(values[i]);
                }
            }
            values.clear();
            values = tempValues;
            tempValues.clear();
        } else {
            std::cerr << "Invalid row index." << std::endl;
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

    void uploadValue(size_t rowIndex, const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }

        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            std::string value1 = "NULL";
            values[rowIndex] = value1;
        } else{
            values[rowIndex] = value;
        }
    }

    void uploadAllValues(const std::string& oldValue, const std::string& newValue) override{
        if (oldValue.empty()) {
            throw std::invalid_argument("Old value cannot be empty.");
        }

        for (size_t i = 0; i < values.size(); ++i) {
            if (!values[i].empty() && values[i] == oldValue) {
                values[i] = newValue;
            }
        }
    }
    int getSize() override{
        return values.size();
    }
    bool containsValue(const std::string& value) override{
        for(const auto val : values ){
            if(value == val){
                return true;
                break;
            }
        }
        return false;
    }
    void deleteValue(size_t rowIndex) override {
        if (rowIndex < values.size()) {
            std::vector<std::string> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (i != rowIndex) {
                    tempValues.push_back(values[i]);
                }
            }
            values.clear();
            values = tempValues;
            tempValues.clear();
        } else {
            std::cerr << "Invalid row index." << std::endl;
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
            std::cout << (values[rowIndex] == 1 ? "true" : "false") << "\t\t";
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
    void uploadValue(size_t rowIndex, const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }

        if((value.empty() && !isNotNullColumn()) || value == "NULL"){
            values[rowIndex] = std::nullopt;
        } else{
            values[rowIndex] = (value == "true");
        }
    }
    void uploadAllValues(const std::string& oldValue, const std::string& newValue) override{
        if (oldValue.empty()) {
            throw std::invalid_argument("Old value cannot be empty.");
        }

        bool newValue2 =(newValue == "true" ? true : false) ;
        bool oldValue2 = (oldValue == "true" ? true: false);
        for (size_t i = 0; i < values.size(); ++i) {
            if (values[i].has_value() && values[i].value() == oldValue2) {
                values[i] = newValue2;
            }
        }
    }
    int getSize() override{
        return values.size();
    }

    bool containsValue(const std::string& value) override{
        bool number = (value == "true" ? true : false);
        for(const auto val : values ){
            if(number == val){
                return true;
                break;
            }
        }
        return false;
    }
    void deleteValue(size_t rowIndex) override {
        if (rowIndex < values.size()) {
            std::vector<std::optional<bool>> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (i != rowIndex) {
                    tempValues.push_back(values[i]);
                }
            }
            values.clear();
            values = tempValues;
            tempValues.clear();
        } else {
            std::cerr << "Invalid row index." << std::endl;
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

    void uploadValue(size_t rowIndex, const std::string& value) override {
        if (value.empty() && isNotNullColumn()) {
            throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
        }

        if((value.empty() && !isNotNullColumn()) || value.size() <= 0){
            std::string value1 = "NULL";
            values[rowIndex] = value1;
        } else{
            values[rowIndex] = value;
        }
    }

    void uploadAllValues(const std::string& oldValue, const std::string& newValue) override{
        if (oldValue.empty()) {
            throw std::invalid_argument("Old value cannot be empty.");
        }

        for (size_t i = 0; i < values.size(); ++i) {
            if (!values[i].empty() && values[i] == oldValue) {
                values[i] = newValue;
            }
        }
    }
    int getSize() override{
        return values.size();
    }
    bool containsValue(const std::string& value) override{
        for(const auto val : values ){
            if(value == val){
                return true;
                break;
            }
        }
        return false;
    }
    void deleteValue(size_t rowIndex) override {
        if (rowIndex < values.size()) {
            std::vector<std::string> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (i != rowIndex) {
                    tempValues.push_back(values[i]);
                }
            }
            values.clear();
            values = tempValues;
            tempValues.clear();
        } else {
            std::cerr << "Invalid row index." << std::endl;
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

    bool containsPK(size_t columnIndex,const std::string& value){
        if(columns[columnIndex]->isPrimaryKey()) {
            return columns[columnIndex]->containsValue(value);
        }
        return false;
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
        bool containPK = false;

        for (size_t i = 0; i < columns.size() ; ++i) {
            if (i >= row.size()) {
                addColumnValue(i, "");
            } else {
                if (containsPK(i, row[i])) {
                    std::cout << "Primary key with this value " << row[i] << " exists!!!" << std::endl;
                    containPK = true;
                    break;
                }
            }
        }
        if (!containPK) {
            for (size_t i = 0; i < columns.size() ; ++i) {
                if (i >= row.size()) {
                        addColumnValue(i, "");
                } else {
                        addColumnValue(i, row[i]);
                }
            }
        }

    }

    void printHeader() const {
        for (const auto& column : columns) {
            column->printHeader();
        }
        std::cout << std::endl;
    }

    void printIndexingColumns() const{
        int i = 0;
        for (const auto& column : columns) {
            std::cout << column->getName() << ": Index: " << i + 1<< std::endl;
            ++i;
        }
        std::cout << std::endl;
    }

    void printRow(size_t rowIndex) const {
        for (const auto& column : columns) {
            column->printValue(rowIndex);
        }
        std::cout << std::endl;
    }

    void pritnAllRows() {
        size_t maxRowCount = 0;

        // Nájdi maximálny počet riadkov pre všetky stĺpce
        for (const auto& column : columns) {
            maxRowCount = std::max(maxRowCount, static_cast<size_t>(column->getSize()));
        }

        // Vypíš všetky riadky
        for (size_t i = 0; i < maxRowCount; ++i) {
            for (const auto& column : columns) {
                if (i < column->getSize()) {
                    column->printValue(i);
                } else {
                    std::cout << "\t\t";
                }
            }
            std::cout << std::endl;
        }
    }
    void uploadColumnValue(size_t columnIndex, size_t rowIndex, const std::string& value) {
        columnIndex -=1;
        rowIndex -= 1;
        if (columnIndex < columns.size()) {
            columns[columnIndex]->uploadValue(rowIndex, value);
        } else {
            std::cerr << "Invalid column index." << std::endl;
        }
    }

    void uploadAllColumnValues(size_t columnIndex, const std::string& oldValue, const std::string& newValue) {
        if (columnIndex < columns.size()) {
            columns[columnIndex]->uploadAllValues(oldValue, newValue);
        } else {
            std::cerr << "Invalid column index." << std::endl;
        }
    }

    void deleteRow(size_t rowIndex) {
        rowIndex -= 1;
        if (rowIndex <= columns.size() && rowIndex>= 0) {
            for (auto& column : columns) {
                column->deleteValue(rowIndex);
            }
            std::cout << "Row at index " << rowIndex + 1 << " deleted." << std::endl;
        } else {
            std::cerr << "Invalid row index." << std::endl;
        }

    }

};

int main() {
    Table myTable("SampleTable");


    myTable.addColumn(new IntColumn("ID", true, true));
    myTable.addColumn(new StringColumn("Name", true, false));
    myTable.addColumn(new DoubleColumn("Salary", false, false));
    myTable.addColumn(new BoolColumn("Active", false, false));
    myTable.addColumn(new DateColumn("Date", false, false));


    myTable.addRow({"1", "John", "50000.0", "true", "25.5.1245"});
    myTable.addRow({"2", "Lopata", "60000.1", "false",""});
    myTable.addRow({"3", "Doe","25.4","",""});
    myTable.addRow({"4","ASDads","25.1","false", "25.4.2023"});
    myTable.addRow({"5","Mali","","","28.4.223"});
    myTable.addRow({"6","SMali","50","true",""});
    myTable.printHeader();
    myTable.pritnAllRows();
    myTable.uploadAllColumnValues(1,"John","Jozo");
    myTable.printHeader();
    myTable.pritnAllRows();;
    myTable.deleteRow(2);
    myTable.addRow({"10","Msdaali","","","8.4.223"});
    myTable.deleteRow(4);
    myTable.deleteRow(2);
    myTable.printHeader();
    myTable.pritnAllRows();
    return 0;
}
