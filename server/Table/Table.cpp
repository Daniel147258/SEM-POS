#include "Table.h"
#include "StringColumn.h"
#include "DoubleColumn.h"


Table::Table(const std::string& name, User* creator) : tableName(name) {
    countRows = 0;
    this->creator = creator;
}

// uprava zavolat desktruktor na zmazanie udajov v column
Table::~Table() {
    for (auto column : columns) {
        delete column;
    }
}

void Table::addColumn(TableColumnBase* column) {
    columns.push_back(column);
}

bool Table::containsPK(size_t columnIndex, const std::string& value) {
    if (columns[columnIndex]->isPrimaryKey()) {
        return columns[columnIndex]->containsValue(value);
    }
    return false;
}

void Table::addColumnValue(size_t columnIndex, const std::string& value) {
    if (columnIndex < columns.size()) {
        columns[columnIndex]->addValue(value);
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }
}

void Table::addRow(const std::vector<std::string>& row) {
    if (row.size() > columns.size()) {
        std::cerr << "Too many elements in the row. Ignoring extra values." << std::endl;
    }
    bool containPK = false;

    for (size_t i = 0; i < columns.size(); ++i) {
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
        ++countRows;
        for (size_t i = 0; i < columns.size(); ++i) {
            if (i >= row.size()) {
                addColumnValue(i, "");
            } else {
                addColumnValue(i, row[i]);
            }
        }
    }
}

void Table::printHeader() const {
    for (const auto& column : columns) {
        column->printHeader();
    }
    std::cout << std::endl;
}

void Table::printIndexingColumns() const {
    int i = 0;
    for (const auto& column : columns) {
        std::cout << column->getName() << ": Index: " << i + 1 << std::endl;
        ++i;
    }
    std::cout << std::endl;
}

void Table::printRow(size_t rowIndex) const {
    for (const auto& column : columns) {
        column->printValue(rowIndex);
    }
    std::cout << std::endl;
}

void Table::pritnAllRows() {
    for (size_t i = 0; i < countRows; ++i) {
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

void Table::uploadColumnValue(size_t columnIndex, size_t rowIndex, const std::string& value) {
    columnIndex -= 1;
    rowIndex -= 1;
    if (columnIndex < columns.size()) {
        columns[columnIndex]->uploadValue(rowIndex, value);
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }
}

int Table::uploadAllColumnValues(size_t columnIndex, const std::string& oldValue, const std::string& newValue) {
    int uptadetRows = 0;
    if (columnIndex < columns.size()) {
        uptadetRows = columns[columnIndex]->uploadAllValues(oldValue, newValue);
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }
    return uptadetRows;
}

void Table::deleteRow(size_t rowIndex) {
    rowIndex -= 1;
    if (rowIndex < columns.size() && rowIndex >= 0) {
        for (auto& column : columns) {
            column->deleteValue(rowIndex);
        }
        std::cout << "Row at index " << rowIndex + 1 << " deleted." << std::endl;
        --countRows;
    } else {
        std::cerr << "Invalid row index." << std::endl;
    }
}

int Table::deleteRowsByValue(size_t columnIndex, const std::string& value) {
    columnIndex -= 1;
    int numberOfDeleted = 0;

    if (columnIndex < columns.size() && columnIndex >= 0) {
        for (size_t rowIndex = 0; rowIndex < columns[columnIndex]->getSize(); ++rowIndex) {
            if (columns[columnIndex]->getValue(rowIndex) == value) {
                // Odstráň hodnoty z všetkých stĺpcov pre daný riadok
                for (auto& column : columns) {
                    column->deleteValue(rowIndex);
                }
                numberOfDeleted++;
            }
        }

        std::cout << numberOfDeleted << " rows deleted where value in column " << columnIndex + 1 << " is '" << value << "'." << std::endl;
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }
    return numberOfDeleted;
}

int Table::getCountRows() {
    return countRows;
}

std::string Table::getName() {
    return tableName;
}

User* Table::getCreator() {
    return creator;
}



std::string Table::getAllRows(){
    std::string a = "No rows in this table";
    if(columns[0]->getSize() != 0){
        a = "";
        for (size_t i = 0; i < countRows; ++i) {
            for (const auto& column : columns) {
                if (i < column->getSize()) {
                    a += column->getValue(i) + "\t\t";
                } else {
                    a += "\t\t";
                }
            }
            a += "\n";
        }
    }
    return a;
}

bool Table::existsColumnIndex(int index){
    return columns.size() <= index;
};

std::string Table::getColumnsIndexes(){
    int i = 0;
    std::string a = "None index";
    if(!columns.empty()) {
        a = "";
        for (const auto &column: columns) {
            a += column->getName() + ": Index " + std::to_string(i+1) + "\n";
            ++i;
        }
    }
    return a;
}

int Table::getNumberOfColumns(){
    return columns.size();
}

std::string Table::getColumnDescription(int index){
    std::string a = "This column dont exist";
    if(index > -1 && index <= columns.size()){
        a = columns[index]->getDescription();
    }
    return a;
}

bool Table::isColumnNullAble(int index){
    if(index >= 0 && index <= columns.size()){
        return columns[index]->isNotNullColumn();
    }
    return false;
}

std::string Table::getColumnType(int index){
    if(index >= 0 && index <= columns.size()){
        return columns[index]->getTypea();
    }
    return "";
}

bool Table::existsInTablePrimaryKey(){
    for(const auto& column : columns) {
        if(column->isPrimaryKey()) {
            return true;
        }
    }
    return false;
}

std::string Table::getRow(int index){
    std::string a;
    for(auto col: columns) {
        if(col->getSize() == 0){
            a = "Wrong size of index!!";
            break;
        }
        else {
            a += col->getValue(index - 1) + "\t\t";
        }
    }
    return a;
}

int main(){
    User* usera = new User("Jozo", "12345");
    Table* table = new Table("name",usera);
    table->addColumn(new StringColumn("Name", true, true));
    table->addColumn(new DoubleColumn("Value", false, false));
    table->addRow({"Jozo", });
    std::cout << table->getColumnsIndexes() << "\n";
    std::cout <<  table->getAllRows() << "\n";
    std::cout << table->getRow(1) << "\n";
}
