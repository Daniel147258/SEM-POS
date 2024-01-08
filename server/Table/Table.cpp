#include "Table.h"
#include "StringColumn.h"
#include "DoubleColumn.h"
#include "IntColumn.h"

Table::Table(const std::string& name, User* creator) : tableName(name) {
    countRows = 0;
    this->creator = creator;
}

Table::~Table() {
    for (auto column : columns) {
        delete column;
    }
    columns.clear();
    selecting.clear();
    updating.clear();
    adding.clear();
    deleting.clear();
}

void Table::addColumn(TableColumnBase* column) {
    std::unique_lock<std::mutex> lock(addColumnMtx);
    columns.push_back(column);
    lock.unlock();
}

bool Table::containsPK(size_t columnIndex, const std::string& value) {
    std::unique_lock<std::mutex> lock(containsPKMtx);
    if (columns[columnIndex]->isPrimaryKey()) {
        lock.unlock();
        return columns[columnIndex]->containsValue(value);
    }
    lock.unlock();
    return false;
}

void Table::addColumnValue(size_t columnIndex, const std::string& value) {
    std::unique_lock<std::mutex> lock(addColumnValueMtx);
    if (columnIndex < columns.size()) {
        columns[columnIndex]->addValue(value);
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }
    lock.unlock();
}

void Table::addRow(const std::vector<std::string>& row) {
    std::unique_lock<std::mutex> lock(addRowMtx);
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
    lock.unlock();
}

void Table::printHeader() const {
    for (const auto& column : columns) {
        column->printHeader();
    }
    std::cout << std::endl;
}

int Table::uploadAllColumnValues(size_t columnIndex, const std::string& oldValue, const std::string& newValue) {
    std::unique_lock<std::mutex> lock(uploadAllColumnValuesMtx);
    int uptadetRows = 0;
    if (columnIndex <= columns.size() && columnIndex >= 0) {
        uptadetRows = columns[columnIndex]->uploadAllValues(oldValue, newValue);
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }
    lock.unlock();
    return uptadetRows;
}


std::string Table::getName() {
    return tableName;
}

User* Table::getCreator() {
    return creator;
}


bool Table::existsColumnIndex(int index){
    std::unique_lock<std::mutex> lock(existsColumnIndexMtx);
    if(index >= 0 && index < columns.size()){
        lock.unlock();
     return true;
    }
    lock.unlock();
    return false;
};

std::string Table::getColumnsIndexes(){
    std::unique_lock<std::mutex> lock(getColumnsIndexesMtx);
    int i = 0;
    std::string a = "None index";
    if(!columns.empty()) {
        a = "";
        for (const auto &column: columns) {
            a += column->getName() + ": Index " + std::to_string(i+1) + "\n";
            ++i;
        }
    }
    lock.unlock();
    return a;
}

int Table::getNumberOfColumns(){
    return columns.size();
}

std::string Table::getColumnDescription(int index){
    std::unique_lock<std::mutex> lock(getColumnDescriptionMtx);
    std::string a = "This column dont exist!!";
    if(index > -1 && index <= columns.size()){
        a = columns[index]->getDescription();
    }
    lock.unlock();
    return a;
}

bool Table::isColumnNullAble(int index){
    std::unique_lock<std::mutex> lock(isColumnNullAbleMtx);
    if(index >= 0 && index <= columns.size()){
        return columns[index]->isNotNullColumn();
    }
    lock.unlock();
    return false;
}

std::string Table::getColumnType(int index){
    std::unique_lock<std::mutex> lock(getColumnTypeMtx);
    if(index >= 0 && index <= columns.size()){
        lock.unlock();
        return columns[index]->getTypea();
    }
    lock.unlock();
    return "";
}

bool Table::existsInTablePrimaryKey(){
    std::unique_lock<std::mutex> lock(existsInTablePrimaryKeyMtx);
    for(const auto& column : columns) {
        if(column->isPrimaryKey()) {
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

int Table::deleteRowsByValue(size_t columnIndex, const std::string& value) {
    std::unique_lock<std::mutex> lock(deleteRowsByValueMtx);
    columnIndex -= 1;
    int numberOfDeleted = 0;
    if (columnIndex < columns.size() && columnIndex >= 0) {
        for(int i = 0; i < countRows; ++i) {
            int row = columns[columnIndex]->deleteValue(value);
            if (row > -1) {
                numberOfDeleted++;
                for (int j = 0; j < columns.size(); ++j) {
                    if (j != columnIndex) {
                        columns[j]->deleteValue(columns[j]->getValue(row));
                    }
                }
            }
        }
    } else {
        std::cerr << "Invalid column index." << std::endl;
    }

    countRows -= numberOfDeleted;
    lock.unlock();
    return numberOfDeleted;
}

std::string Table::getRow(int index){
    std::unique_lock<std::mutex> lock(getRowMtx);
    std::string a;
    index -= 1;
    for(const auto& col: columns) {
        if(col->getSize() == 0 || index >= columns.size() || index < 0){
            a = "Wrong size of index!!";
            break;
        }
        else {
            a += "|"+ col->getValue(index) + "|\t\t";
        }
    }
    lock.unlock();
    return getHeader() + "\n" + a + "\n";
}

std::string Table::getAllRows(){
    std::unique_lock<std::mutex> lock(getAllRowsMtx);
    std::string a = "No rows in this table";
    if(countRows != 0){
        a = "";
        for (int i = 0; i < countRows; ++i) {
            for (const auto& column : columns) {
                if (i < column->getSize()) {
                    a += "| "+column->getValue(i) + " |\t\t";
                }
            }
            a += "\n";
        }
    }
    lock.unlock();
    return getHeader() + "\n" + a + "\n";
}

std::string Table::getHeader(){
    std::unique_lock<std::mutex> lock(getHeaderMtx);
    std::string a = "Table " + getName() + ":\n";
    for(auto col: columns) {
        a += col->getName() +  "\t\t";
    }
    lock.unlock();
    return a;
}

bool Table::isColumnPrimaryKer(int index){
    std::unique_lock<std::mutex> lock(isColumnPrimaryKerMtx);
    bool a = columns[index]->isPrimaryKey();
    lock.unlock();
    return a;
}

bool Table::canUserSelect(User *client) {
    std::unique_lock<std::mutex> lock(canUserSelectMtx);
    if(client->getMeno() == creator->getMeno()){
        lock.unlock();
        return true;
    }
    for(auto clie : selecting){
        if(client->getMeno() == clie->getMeno() ){
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

bool Table::canUserUpdate(User *client) {
    std::unique_lock<std::mutex> lock(canUserUpdateMtx);
    if(client->getMeno() == creator->getMeno()){
        lock.unlock();
        return true;
    }
    for(auto clie : updating){
        if(client->getMeno() == clie->getMeno() ){
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

bool Table::canUserAdd(User *client) {
    std::unique_lock<std::mutex> lock(canUserAddMtx);
    if(client->getMeno() == creator->getMeno()){
        lock.unlock();
        return true;
    }
    for(auto clie : adding){
        if(client->getMeno() == clie->getMeno() ){
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

bool Table::canUserDelete(User *client) {
    std::unique_lock<std::mutex> lock(canUserDeleteMtx);
    if(client->getMeno() == creator->getMeno()){
        lock.unlock();
        return true;
    }
    for(auto clie : deleting){
        if(client->getMeno() == clie->getMeno() ){
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

std::string Table::getListOfUserRights(User *client) {
    std::unique_lock<std::mutex> lock(getListOfUserRightsMtx);
    std::string a = "Table " + getName() + ": You Dont have any right";
    bool isSomething = false;
    if(getCreator()->getMeno() == client->getMeno()){
        a = "Table " + getName() + ": You are creator\n";
        lock.unlock();
        return a;
    }
    for(auto us : selecting){
        if(us->getMeno() == client->getMeno()){
                a = "Table " + getName() + ": SELECT, ";
                isSomething = true;
            break;
        }
    }

    for(auto us : updating){
        if(us->getMeno() == client->getMeno()){
            if(!isSomething){
                a = "Table " + getName() + ": UPDATE, ";
                isSomething = true;
            } else{
                a += "UPDATE, ";
            }
            break;
        }
    }

    for(auto us : adding){
        if(us->getMeno() == client->getMeno()){
            if(!isSomething){
                a = "Table " + getName() + ": ADD, ";
                isSomething = true;
            } else{
                a += "ADD, ";
            }
            break;
        }
    }

    for(auto us : deleting){
        if(us->getMeno() == client->getMeno()){
            if(!isSomething){
                a = "Table " + getName() + ": DELETE, ";
                isSomething = true;
            } else{
                a += "DELETE, ";
            }
            break;
        }
    }
    lock.unlock();
    return a + "\n";
}

std::string Table::addRightSelect(User *client) {
    std::unique_lock<std::mutex> lock(addRightSelectMtx);
    std::string a = "The problem is that the rights are not applied!!";
    if(!client){
        a = "Client dont exists !!";
        lock.unlock();
        return a;
    }
    for(auto u : selecting){
        if(u->getMeno() == client->getMeno()){
            a = "User already have this right";
            lock.unlock();
            return a;
        }
    }
    selecting.push_back(client);
    a = "Right successfully added\n";
    lock.unlock();
    return a;
}

std::string Table::addRightUpdate(User *client) {
    std::unique_lock<std::mutex> lock(addRightUpdateMtx);
    std::string a = "The problem is that the rights are not applied!!";
    if(!client){
        a = "Client dont exists !!";
        lock.unlock();
        return a;
    }
    for(auto u : updating){
        if(u->getMeno() == client->getMeno()){
            a = "User already have this right";
            lock.unlock();
            return a;
        }
    }
    updating.push_back(client);
    a = "Right successfully added\n";
    lock.unlock();
    return a;
}

std::string Table::addRightAdd(User *client) {
    std::unique_lock<std::mutex> lock(addRightAddMtx);
    std::string a = "The problem is that the rights are not applied!!";
    if(!client){
        a = "Client dont exists !!";
        lock.unlock();
        return a;
    }
    for(auto u : adding){
        if(u->getMeno() == client->getMeno()){
            a = "User already have this right";
            lock.unlock();
            return a;
        }
    }
    adding.push_back(client);
    a = "Right successfully added\n";
    lock.unlock();
    return a;
}

std::string Table::addRightDelete(User *client) {
    std::unique_lock<std::mutex> lock(addRightDeleteMtx);
    std::string a = "The problem is that the rights are not applied!!";
    if(!client){
        a = "Client dont exists !!";
        lock.unlock();
        return a;
    }
    for(auto u : deleting){
        if(u->getMeno() == client->getMeno()){
            a = "User already have this right";
            lock.unlock();
            return a;
        }
    }
    deleting.push_back(client);
    a = "Right successfully added\n";
    lock.unlock();
    return a;
}

std::string Table::getColumnToCsvFile() {
    std::string a;
    for(auto col : columns){
        a += col->getTypea() + ";" + col->getName() + ";" + (col->isPrimaryKey() ? "true" : "false") + ";" + (col->isNotNullColumn() ? "true" : "false") + "\n";
    }
    return a;
}

std::string Table::getRowsToCsvFile() {
    std::string a;
    for (int i = 0; i < countRows; ++i) {
        int b = 1;
        for(auto col : columns){
            if(b == 1){
                a += col->getValue(i)+ ";";
            } else if(b < columns.size()) {
                a += col->getValue(i) + ";";
            } else{
                a += col->getValue(i);
            }
            b++;
        }
        a += "\n";
    }
    return a;
}

std::string Table::getRightsToCsvFile() {
    std::string a;
    for(auto col : selecting){
        a += "SELECT;" + col->getMeno() + "\n";
    }
    for(auto col : updating){
        a += "UPDATE;" + col->getMeno() + "\n";
    }
    for(auto col : adding){
        a += "ADD;" + col->getMeno() + "\n";
    }
    for(auto col : deleting){
        a += "DELETE;" + col->getMeno() + "\n";
    }
    return a;
}

void Table::setCountOfRows() {
    if(columns.empty()){
        countRows = 0;
    }
    countRows = columns[0]->getSize();
}


