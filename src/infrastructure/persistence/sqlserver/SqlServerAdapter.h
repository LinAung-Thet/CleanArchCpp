#pragma once

#include "../IServerHelper.h"
#include "SqlServerHelper.h"

namespace infrastructure::persistence::sqlserver {

template<typename T>
class SqlServerAdapter : public IServerHelper<T> {
public:
    std::string buildInsert(const T& entity) override {
        return SqlServerHelper::buildInsertSql<T>();
    }
};

}
