#include "StringColumn.h"
#include <iostream>

StringColumn::StringColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
        : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

void StringColumn::printHeader() const {
    std::cout << getName() << " (string)\t";
}

void StringColumn::printValue(size_t rowIndex) const {
    std::cout << values[rowIndex] << "\t\t";
}

void StringColumn::addValue(const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }
    if ((value.empty() && !isNotNullColumn()) || value.size() <= 0) {
        std::string value1 = "NULL";
        values.push_back(value1);
    } else {
        values.push_back(value);
    }
}

void StringColumn::uploadValue(size_t rowIndex, const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }

    if ((value.empty() && !isNotNullColumn()) || value.size() <= 0) {
        std::string value1 = "NULL";
        values[rowIndex] = value1;
    } else {
        values[rowIndex] = value;
    }
}

int StringColumn::uploadAllValues(const std::string& oldValue, const std::string& newValue) {
    if (oldValue.empty()) {
        throw std::invalid_argument("Old value cannot be empty.");
    }
    int uptadetRows = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (!values[i].empty() && values[i] == oldValue) {
            values[i] = newValue;
            uptadetRows++;
        }
    }
    return uptadetRows;
}

int StringColumn::getSize() {
    return values.size();
}

bool StringColumn::containsValue(const std::string& value) {
    for (const auto val : values) {
        if (value == val) {
            return true;
        }
    }
    return false;
}

int StringColumn::deleteValue(const std::string& value){
    int deleted = -1;
    try {
        std::string number = value;
        if (values.size() > 0) {
            std::vector<std::string> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (values[i] != number) {
                    tempValues.push_back(values[i]);
                }
                else{
                    deleted = i;
                    break;
                };
            }
            values.clear();
            for (size_t i = 0; i < tempValues.size(); ++i) {
                values.push_back(tempValues[i]);
            }
            tempValues.clear();
        }
    }
    catch(const std::invalid_argument& e){
        std::cerr << "Wrong value!!\n";
    }
    return deleted;
}

std::string StringColumn::getValue(size_t rowIndex) const {
    if (!values[rowIndex].empty()) {
        return values[rowIndex];
    } else {
        return "NULL";
    }
}

std::string StringColumn::getDescription(){
    return "Name: " + getName() + ", Type: (string)" + " , is not Null: " + std::to_string(isNotNullColumn());
}

std::string StringColumn::getTypea(){
    return "string";
}

StringColumn::~StringColumn() {
    values.clear();
}

