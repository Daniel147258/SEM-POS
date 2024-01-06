#include "TableColumnBase.h"

TableColumnBase::TableColumnBase(const std::string& columnName, bool isNotNull, bool isPrimaryKey)
        : name(columnName), notNull(isNotNull), primaryKey(isPrimaryKey) {
    if (primaryKey) {
        notNull = true;
    }
}

TableColumnBase::~TableColumnBase() {}

std::string TableColumnBase::getName() const {
    return name;
}

bool TableColumnBase::isPrimaryKey() {
    return primaryKey;
}

bool TableColumnBase::isNotNullColumn() {
    return notNull;
}
