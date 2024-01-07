#ifndef DOUBLECOLUMN_H
#define DOUBLECOLUMN_H

#include "TableColumnBase.h"
#include <vector>
#include <optional>
#include <stdexcept>

class DoubleColumn : public TableColumnBase {
private:
    std::vector<std::optional<double>> values;

public:
    DoubleColumn(const std::string& columnName, bool isNotNull, bool isPrimaryKey);
    ~DoubleColumn() override;

    void printHeader() const override;
    void printValue(size_t rowIndex) const override;
    void addValue(const std::string& value) override;
    void uploadValue(size_t rowIndex, const std::string& value) override;
    int uploadAllValues(const std::string& oldValue, const std::string& newValue) override;
    int getSize() override;
    bool containsValue(const std::string& value) override;
    bool deleteValue(const std::string& value) override;
    std::string getValue(size_t rowIndex) const override;
    std::string getDescription() override;
    std::string getTypea() override;
};

#endif // DOUBLECOLUMN_H
