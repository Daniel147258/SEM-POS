#include "IntColumn.h"
#include <iostream>

IntColumn::IntColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
        : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

void IntColumn::printHeader() const {
    std::cout << getName() << " (int)\t";
}

void IntColumn::printValue(size_t rowIndex) const {
    if (values[rowIndex].has_value()) {
        std::cout << values[rowIndex].value() << "\t\t";
    } else {
        std::cout << "NULL\t\t";
    }
}

void IntColumn::addValue(const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }
    if ((value.empty() && !isNotNullColumn()) || value.size() <= 0) {
        values.push_back(std::nullopt);
    } else {
        try {
            int parsedValue = std::stoi(value);
            values.push_back(parsedValue);
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
}

void IntColumn::uploadValue(size_t rowIndex, const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }

    if ((value.empty() && !isNotNullColumn()) || value.size() <= 0) {
        values[rowIndex] = std::nullopt;
    } else {
        try {
            int parsedValue = std::stoi(value);
            values[rowIndex] = parsedValue;
        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
}

int IntColumn::uploadAllValues(const std::string& oldValue, const std::string& newValue) {
    if (oldValue.empty()) {
        throw std::invalid_argument("Old value cannot be empty.");
    }

    int uptadetRows = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        try {
            int parsedValue = std::stoi(oldValue);
            int newValueParsed = std::stoi(newValue);
            if (values[i].has_value() && values[i].value() == parsedValue) {
                values[i] = newValueParsed;
                uptadetRows++;
            }
        }
        catch (const std::invalid_argument& e) {
        std::cerr << "Invalid argument: " << e.what() << std::endl;
         } catch (const std::out_of_range& e) {
        std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
    return uptadetRows;
}

int IntColumn::getSize() {
    return values.size();
}

bool IntColumn::containsValue(const std::string& value) {
    int number = std::stoi(value);
    for (const auto val : values) {
        if (number == val) {
            return true;
        }
    }
    return false;
}

int IntColumn::deleteValue(const std::string& value){
    int deletedRow = -1;
    try {
        int number = std::stoi(value);
        if (values.size() > 0) {
            std::vector<std::optional<int>> tempValues;
            for (size_t i = 0; i < values.size(); ++i) {
                if (values[i].has_value() && values[i].value() != number) {
                    tempValues.push_back(values[i]);
                }
                else{
                    deletedRow = i;
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
    return deletedRow;
}

std::string IntColumn::getValue(size_t rowIndex) const {
    if (values[rowIndex].has_value()) {
        return std::to_string(values[rowIndex].value());
    } else {
        return "NULL";
    }
}
std::string IntColumn::getDescription(){
    return "Name: " + getName() + ", Type: (int)"+ " , is not Null: " + std::to_string(isNotNullColumn());
}

std::string IntColumn::getTypea(){
    return "int";
}

IntColumn::~IntColumn() {
    values.clear();
}
