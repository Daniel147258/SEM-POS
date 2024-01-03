#pragma once
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
    void createTable(const string& name);
    bool existsTable(const string& name);
};