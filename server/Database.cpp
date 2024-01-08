#include "database.h"
#include "thread"
#include "Table/StringColumn.h"
#include "Table/IntColumn.h"
#include "Table/DoubleColumn.h"
#include "Table/BoolColumn.h"
#include "Table/DateColumn.h"
#include <algorithm>

Database::Database() {
    loadData();
}

Database::~Database() {
    for(auto tab : tables){
        if(tab != nullptr){
            delete tab;
        }
    }
    users.clear();
}

void Database::addTable(Table* table) {
    if (!existTable(table->getName())) {
        std::unique_lock<std::mutex> lock(addTableMtx);
        tables.push_back(table);
        lock.unlock();
    }else{
        std::unique_lock<std::mutex> lock(addTableMtx);
        std::cerr << "Error with add table to database!!\n";
        lock.unlock();
    }
}

bool Database::existTable(const std::string& name)  {
    std::unique_lock<std::mutex> lock(existTableMtx);
    for (auto table : tables) {
        if (table->getName() == name) {
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

void Database::addUser(const std::string& name, const std::string& password) {
    std::unique_lock<std::mutex> lock(existsUser);
    if (!existUsername(name)) {
        users.push_back(new User(name, password));
    }
    lock.unlock();
}

bool Database::existUsername(const std::string& name) {
    std::unique_lock<std::mutex> lock(addUsera);
    for (auto usera : users) {
        if (usera->getMeno() == name) {
            lock.unlock();
            return true;
        }
    }
    lock.unlock();
    return false;
}

User* Database::getUser(const std::string& name) {
    std::unique_lock<std::mutex> lock(getUserMtx);
    for (auto usera : users) {
        if (usera->getMeno() == name) {
            lock.unlock();
            return usera;
        }
    }
    lock.unlock();
    return nullptr;
}

std::string Database::getTables(){
    std::unique_lock<std::mutex> lock(getTablesMtx);
    std::string a = "Not tabels yet";
    int i = 0;
    if(!tables.empty()) {
        a = "";
        for (auto tab: tables) {
            if (i % 10 == 0) {
                a += tab->getName() + ",\n";
            } else {
                a += tab->getName() + ", ";
            }
            ++i;
        }
    }
    lock.unlock();
    return a;
}

Table* Database::getTable(const std::string& name){
    std::unique_lock<std::mutex> lock(getTableMtx);
    for(auto tab : tables){
        if(tab->getName() == name){
            lock.unlock();
            return tab;
        }
    }
    lock.unlock();
    return nullptr;
}

std::string Database::getRowByIndex(const std::string& name, int indexOfRow){
    return getTable(name)->getRow(indexOfRow);
}

std::string Database::getAllRows(const std::string& nameTable){
    return getTable(nameTable)->getAllRows();
}

bool Database::existsColumnIndex(const std::string& nameTable, int index){
    return getTable(nameTable)->existsColumnIndex(index);
}

std::string Database::getColumnsIndexes(const std::string& tableName){
    return getTable(tableName)->getColumnsIndexes();
}

int Database::getNumberOfColumns(const std::string& tableName){
    return getTable(tableName)->getNumberOfColumns();
}

std::string Database::getColumnDescription(const std::string& tableName, int index){
    return getTable(tableName)->getColumnDescription(index);
}

bool Database::isColumnNullAble(const std::string& tableName, int index){
    return getTable(tableName)->isColumnNullAble(index);
}

std::string Database::getTypeOfColumn(const std::string& tableName, int index){
    return getTable(tableName)->getColumnType(index);
}

int Database::DeleteRow(const std::string& tableName, int index, const std::string& value){
    return getTable(tableName)->deleteRowsByValue(index, value);
}

void Database::addRow(const std::string& name, std::vector<std::string>& row){
    getTable(name)->addRow(row);
}

std::string Database::deleteTable(const std::string& name, User* client) {
    std::unique_lock<std::mutex> lock(deleteTableMtx);
    std::string a = "Table doesnt exists !!";
    Table* table = getTable(name);
    if(table){
        if(table->getCreator()->getMeno() == client->getMeno()){
            a = "Table successfully removed";
            removeTableByName(name);
            delete table;
        } else{
            a = "You don't have the right to delete the table !!";
        }
    }
    lock.unlock();
    return a;
}

//nemusi mat mutex vola sa s triedy o mutexovanej
void Database::removeTableByName(const std::string& name) {
    auto it = std::remove_if(tables.begin(), tables.end(),
                             [name](Table* table) {
                                 return name == table->getName();
                             });

    tables.erase(it, tables.end());
}

std::string Database::getUserTables(User *client) {
    std::unique_lock<std::mutex> lock(getUserTablesMtx);
    std::string a = "You dont have any Table\n";
    bool first = false;
    for(auto tab: tables){
        if(tab->getCreator()->getMeno() == client->getMeno()){
            if(!first){
                first = true;
                a = "Your Tables:\n" + tab->getName() + ", ";
            }
            else{
                a += tab->getName() + ", ";
            }
        }
    }
    lock.unlock();
    return a;
}

std::string Database::getUserTablesWithRights(User *client) {
    std::unique_lock<std::mutex> lock(getUserTablesWithRightsMtx);
    std::string a = "Tables with your rights:\n";
    int i = 1;
    if(tables.size() == 0){
        a += "You dont have any right to table\n";
        lock.unlock();
        return a;
    }
    else{
        for(auto tab : tables) {
            a += std::to_string(i) + ". "+tab->getListOfUserRights(client);
            i++;
        }
    }
    lock.unlock();
    return a;
}

//moze ostat bez mutexu
bool Database::isTableUser(const std::string &tableName, User *client) {
    return getTable(tableName)->getCreator()->getMeno() == client->getMeno();
}

void Database::writeData() {
    //Zapisanie uzivatelov
    std::ofstream file("C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\Users\\UsersData.csv", std::ios::trunc);
    if (file.is_open()) {
        for(auto u: users) {
            file << u->getMeno() << ";" << u->getHeslo() << "\n";
        }
        file.close();
    }
    else {
        std::cerr << "File cannot be open";
    }

    //Zapisanie tabuliek
    file.open("C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\Tables.csv", std::ios::trunc);
    if (file.is_open()) {
        for(auto tab: tables) {
            file << tab->getName()<< ";" << tab->getCreator()->getMeno() << "\n";
        }
        file.close();
    }
    else {
        std::cerr << "File cannot be open";
    }

    //Zapisanie Stlpcov do tabuliek
    for(auto tab: tables) {
        std::string path = "C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\" + tab->getName() + "Columns.csv";
        file.open(path,std::ios::trunc);
        if (file.is_open()) {
            file << tab->getColumnToCsvFile();
            file.close();
        }
        else {
            std::cerr << "File cannot be open";
        }
    }

    //Zapisanie riadkov
    for(auto tab: tables) {
        std::string path = "C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\" + tab->getName() + "Rows.csv";
        file.open(path, std::ios::trunc);
        if (file.is_open()) {
            file << tab->getRowsToCsvFile();
            file.close();
        } else {
            std::cerr << "File cannot be open";
        }
    }

    //zapisanie prav pre uzivatelov
    for(auto tab: tables) {
        std::string path = "C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\" + tab->getName() + "Rights.csv";
        file.open(path,std::ios::trunc);
        if (file.is_open()) {
            file << tab->getRightsToCsvFile();
            file.close();
        }
        else {
            std::cerr << "File cannot be open";
        }
    }

}

//moze ostat bez mutexu
void Database::loadData() {
    std::ifstream file("C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\Users\\UsersData.csv");

    //nacitanie uzivatelov
    if (!file.is_open()) {
        std::cerr << "File not found or cannot be opened.\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, password;
        if (std::getline(iss, name, ';') && std::getline(iss, password)) {
            users.push_back(new User(name, password));
        } else {
            std::cerr << "Invalid data format in the file.\n";
        }
    }
    file.close();

    //nacitanie tabuliek
    file.open("C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\Tables.csv");
    if (!file.is_open()) {
        std::cerr << "File not found or cannot be opened.\n";
        return;
    }
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string tableName, creatorName;
        if (std::getline(iss, tableName, ';') && std::getline(iss, creatorName)) {
            tables.push_back(new Table(tableName, getUser(creatorName)));
        } else {
            std::cerr << "Invalid data format in the file.\n";
        }
    }
    file.close();

    //nacitanie stlpcov do tabuliek
    for(auto tab : tables) {
        std::string path = "C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\" +tab->getName() + "Columns.csv";
        file.open(path);
        if (!file.is_open()) {
            std::cerr << "File not found or cannot be opened.\n";
            return;
        }
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type, columnName, primaryKey, notNull;
            if (std::getline(iss, type, ';') && std::getline(iss, columnName, ';') && std::getline(iss, primaryKey, ';') && std::getline(iss, notNull)) {
                if(type == "string"){
                    bool isPrimaryKey = (primaryKey == "true");
                    bool isNotNull = (notNull == "true");
                    tab->addColumn(new StringColumn(columnName, isNotNull, isPrimaryKey));
                } else if(type == "int"){
                    bool isPrimaryKey = (primaryKey == "true");
                    bool isNotNull = (notNull == "true");
                    tab->addColumn(new IntColumn(columnName, isNotNull, isPrimaryKey));
                } else if(type == "double"){
                    bool isPrimaryKey = (primaryKey == "true");
                    bool isNotNull = (notNull == "true");
                    tab->addColumn(new DoubleColumn(columnName, isNotNull, isPrimaryKey));
                } else if (type == "bool"){
                    bool isPrimaryKey = (primaryKey == "true");
                    bool isNotNull = (notNull == "true");
                    tab->addColumn(new BoolColumn(columnName, isNotNull));
                } else if (type == "date"){
                    bool isPrimaryKey = (primaryKey == "true");
                    bool isNotNull = (notNull == "true");
                    tab->addColumn(new DateColumn(columnName, isNotNull));
                } else{
                    std::cerr << "Invalid type in this row.\n";
                }
            } else {
                std::cerr << "Invalid data format in the file.\n";
            }
        }
        file.close();
    }

    //nacitanie riadkov
    for(auto tab : tables) {
        std::string path = "C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\" +tab->getName() + "Rows.csv";
        file.open(path);
        if (!file.is_open()) {
            std::cerr << "File not found or cannot be opened.\n";
            return;
        }
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::vector<std::string> values;
            std::string value;

            while (std::getline(iss, value, ';')) {
                values.push_back(value);
            }
            tab->addRow(values);
            values.clear();
        }
        tab->setCountOfRows();
        file.close();
    }

    //nacitanie prav
    for(auto tab : tables) {
        std::string path = "C:\\Users\\Daniel\\CLionProjects\\Sem\\server\\Data\\TableData\\" +tab->getName() + "Rights.csv";
        file.open(path);
        if (!file.is_open()) {
            std::cerr << "File not found or cannot be opened.\n";
            return;
        }
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string type, name;
            if (std::getline(iss, type, ';') && std::getline(iss, name)) {
                if(type == "SELECT"){
                    tab->addRightSelect(getUser(name));
                } else if(type == "UPDATE"){
                    tab->addRightUpdate(getUser(name));
                } else if(type == "ADD"){
                    tab->addRightAdd(getUser(name));
                } else if (type == "DELETE"){
                    tab->addRightDelete(getUser(name));
                } else{
                    std::cerr << "Invalid type in this row.\n";
                }
            } else {
                std::cerr << "Invalid data format in the file.\n";
            }
        }
        file.close();
    }
}



