
#include "DateColumn.h"
#include <iostream>
#include <stdexcept>

DateColumn::DateColumn(const std::string& columnName, bool isNotNull)
        : TableColumnBase(columnName, isNotNull, false), values() {}

void DateColumn::printHeader() const {
    std::cout << getName() << " (date)\t";
}

void DateColumn::printValue(size_t rowIndex) const {
    std::cout << values[rowIndex] << "\t\t";
}

void DateColumn::addValue(const std::string& value) {
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

void DateColumn::uploadValue(size_t rowIndex, const std::string& value) {
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

int DateColumn::uploadAllValues(const std::string& oldValue, const std::string& newValue) {
    if (oldValue.empty()) {
        throw std::invalid_argument("Old value cannot be empty.");
    }
    int updatedRows = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        if (!values[i].empty() && values[i] == oldValue) {
            values[i] = newValue;
            updatedRows++;
        }
    }
    return updatedRows;
}

int DateColumn::getSize() {
    return values.size();
}

bool DateColumn::containsValue(const std::string& value) {
    for (const auto val : values) {
        if (value == val) {
            return true;
            break;
        }
    }
    return false;
}

void DateColumn::deleteValue(size_t rowIndex) {
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

std::string DateColumn::getValue(size_t rowIndex) const {
    if (!values[rowIndex].empty()) {
        return values[rowIndex];
    } else {
        return "NULL";
    }
}

std::string DateColumn::getDescription(){
    return "Name: " + getName() + ", Type: (date), nullable: " + std::to_string(isNotNullColumn());
}

std::string DateColumn::getTypea(){
    return "date";
}