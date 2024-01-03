#include <iostream>
#include <string>
#include <vector>
#include <optional>
#include <stdexcept>
#include <chrono>
#include "./Table/Table.h"

using namespace std;

class Database{
private:
    vector<Table*> tables;
public:
    void createTable(const string& name){
        if(!existTable(name)){
            Table* table = new Table(name);
            tables.push_back(table);
        }
    }

    bool existTable(const string& name) const{
        for (auto table: tables) {
            if(table->getName() == name){
                return true;
            }
        }
        return false;
    }


};

