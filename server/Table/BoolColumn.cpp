#include "BoolColumn.h"

BoolColumn::BoolColumn(const std::string& columnName, bool isNotNull)
        : TableColumnBase(columnName, isNotNull, false), values() {
}

void BoolColumn::printHeader() const {
    std::cout << getName() << " (bool)\t";
}

void BoolColumn::printValue(size_t rowIndex) const {
    if (values[rowIndex].has_value()) {
        std::cout << (values[rowIndex] == true ? "true" : "false") << "\t\t";
    } else {
        std::cout << "NULL\t\t";
    }
}

void BoolColumn::addValue(const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }
    if ((value.empty() && !isNotNullColumn()) || value == "NULL") {
        values.push_back(std::nullopt);
    } else {
        values.push_back(value == "true" ? true : false);
    }
}

void BoolColumn::uploadValue(size_t rowIndex, const std::string& value) {
    if (value.empty() && isNotNullColumn()) {
        throw std::invalid_argument("NULL value not allowed for NOT NULL column.");
    }

    if ((value.empty() && !isNotNullColumn()) || value == "NULL") {
        values[rowIndex] = std::nullopt;
    } else {
        values[rowIndex] = (value == "true");
    }
}

int BoolColumn::uploadAllValues(const std::string& oldValue, const std::string& newValue) {
    if (oldValue.empty()) {
        throw std::invalid_argument("Old value cannot be empty.");
    }
    int updatedRows = 0;
    bool newValue2 = (newValue == "true");
    bool oldValue2 = (oldValue == "true");
    for (size_t i = 0; i < values.size(); ++i) {
        if (values[i].has_value() && values[i].value() == oldValue2) {
            values[i] = newValue2;
            updatedRows++;
        }
    }
    return updatedRows;
}

int BoolColumn::getSize() {
    return values.size();
}

bool BoolColumn::containsValue(const std::string& value) {
    bool number = (value == "true");
    for (const auto val : values) {
        if (number == val) {
            return true;
        }
    }
    return false;
}

void BoolColumn::deleteValue(size_t rowIndex) {
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

std::string BoolColumn::getValue(size_t rowIndex) const {
    if (values[rowIndex].has_value()) {
        return std::to_string(values[rowIndex].value());
    } else {
        return "NULL";
    }
}

std::string BoolColumn::getDescription(){
    return "Name: " + getName() + ", Type: (bool)" + " , nullable: " + std::to_string(isNotNullColumn());
}

std::string BoolColumn::getTypea(){
    return "bool";
}
