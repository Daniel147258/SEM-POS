#ifndef DATECOLUMN_H
#define DATECOLUMN_H

#include "TableColumnBase.h"
#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <stdexcept>

class DateColumn : public TableColumnBase {
private:
    std::vector<std::string> values;

public:
    DateColumn(const std::string& columnName, bool isNotNull);
    ~DateColumn() override;

    void printHeader() const override;
    void printValue(size_t rowIndex) const override;
    void addValue(const std::string& value) override;
    void uploadValue(size_t rowIndex, const std::string& value) override;
    int uploadAllValues(const std::string& oldValue, const std::string& newValue) override;
    int getSize() override;
    bool containsValue(const std::string& value) override;
    int deleteValue(const std::string& value) override;
    std::string getValue(size_t rowIndex) const override;
    std::string getDescription() override;
    std::string getTypea() override;
};

#endif  // DATECOLUMN_H