#ifndef BOOLCOLUMN_H
#define BOOLCOLUMN_H

#include "TableColumnBase.h"
#include <vector>
#include <optional>
#include <string>
#include <iostream>
#include <stdexcept>

class BoolColumn : public TableColumnBase {
private:
    std::vector<std::optional<bool>> values;

public:
    BoolColumn(const std::string& columnName, bool isNotNull);
    ~BoolColumn() override;

    void printHeader() const override;
    void printValue(size_t rowIndex) const override;
    void addValue(const std::string& value) override;
    void uploadValue(size_t rowIndex, const std::string& value) override;
    int uploadAllValues(const std::string& oldValue, const std::string& newValue) override;
    int getSize() override;
    bool containsValue(const std::string& value) override;
    int deleteValue(const std::string& value) override;
    std::string getValue(int rowIndex)  override;
    std::string getDescription() override;
    std::string getTypea() override;
};

#endif // BOOLCOLUMN_H
