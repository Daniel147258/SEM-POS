#include "DoubleColumn.h"
#include <iostream>

DoubleColumn::DoubleColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
        : TableColumnBase(columnName, isNotNull, isPrimaryKey), values() {}

void DoubleColumn::printHeader() const {
    std::cout << getName() << " (double)\t";
}

void DoubleColumn::printValue(size_t rowIndex) const {
    if (values[rowIndex].has_value()) {
        std::cout << values[rowIndex].value() << "\t\t";
    } else {
        std::cout << "NULL\t\t";
    }
}

void DoubleColumn::addValue(const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }
    if ((value.empty() && !isNotNullColumn()) || value.size() <= 0) {
        values.push_back(std::nullopt);
    } else {
        try {
            double parsedValue = std::stod(value);
            values.push_back(parsedValue);

        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
}

void DoubleColumn::uploadValue(size_t rowIndex, const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }

    if ((value.empty() && !isNotNullColumn()) || value.size() <= 0) {
        values[rowIndex] = std::nullopt;
    } else {
        try {
            double parsedValue = std::stod(value);
            values[rowIndex] = parsedValue;

        } catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: " << e.what() << std::endl;
        } catch (const std::out_of_range& e) {
            std::cerr << "Out of range: " << e.what() << std::endl;
        }
    }
}

int DoubleColumn::uploadAllValues(const std::string& oldValue, const std::string& newValue) {
    if (oldValue.empty()) {
        throw std::invalid_argument("Old value cannot be empty.");
    }

    int uptadetRows = 0;
    for (size_t i = 0; i < values.size(); ++i) {
        try {
            double parsedValue = std::stod(oldValue);
            double newValueParsed = std::stod(newValue);
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

int DoubleColumn::getSize() {
    return values.size();
}

bool DoubleColumn::containsValue(const std::string& value) {
    double number = std::stod(value);
    for (const auto val : values) {
        if (number == val) {
            return true;
        }
    }
    return false;
}

void DoubleColumn::deleteValue(size_t rowIndex) {
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

std::string DoubleColumn::getValue(size_t rowIndex) const {
    if (values[rowIndex].has_value()) {
        return std::to_string(values[rowIndex].value());
    } else {
        return "NULL";
    }
}

std::string DoubleColumn::getDescription(){
    return "Name: " + getName() + ", Type: (double)" + " , nullable: " + std::to_string(isNotNullColumn());
}

std::string DoubleColumn::getTypea(){
    return "double";
}
