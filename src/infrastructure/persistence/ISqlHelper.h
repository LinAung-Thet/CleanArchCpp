#pragma once
#include <string>

#include "SqlTraits.h"

namespace infrastructure::persistence
{

class ISqlHelper
{
public:
    virtual ~ISqlHelper() = default;

    virtual std::string buildInsertSql(
        const SqlMetadata& metadata) const = 0;

    virtual std::string buildSelectSql(
        const SqlMetadata& metadata,
        const std::string& whereClause = "") const = 0;

    virtual std::string buildUpdateSql(
        const SqlMetadata& metadata,
        const std::string& whereClause) const = 0;

    virtual std::string buildDeleteSql(
        const SqlMetadata& metadata,
        const std::string& whereClause) const = 0;

    virtual void executeNonQuery(
        const std::string& sql,
        const SqlParameters& parameters) = 0;

    virtual SqlRows executeQuery(
        const std::string& sql,
        const SqlParameters& parameters) = 0;
};

}