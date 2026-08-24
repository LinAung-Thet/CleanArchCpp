#pragma once
#include <vector>

#include "ISqlHelper.h"
#include "SqlTraits.h"
#include "UserSqlTraits.h"
/* 
How to organize the headers so that DbAdapter doesn't have to explicitly include every 
 UserSqlTraits.h, ProductSqlTraits.h, etc.
*/
namespace infrastructure::persistence
{

template<typename T>
class DbAdapter
{
/*
✓ knows T
✓ executes CRUD operations through IServerHelper
✓ converts returned rows into T
*/

public:

    explicit DbAdapter(
        ISqlHelper& sqlHelper)
        : sqlHelper_(sqlHelper)
    {
    }

    void DbAdapter<T>::add(const T& entity)
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildInsertSql(metadata);

        const auto parameters =
            SqlTraits<T>::toParameters(entity);

        sqlHelper_.executeNonQuery(
            sql,
            parameters);
    }

    std::vector<T>
    DbAdapter<T>::findByColumn(
        const std::string& column,
        const std::string& value)
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildSelectSql(
                metadata,
                column + " = ?");

        const auto rows =
            sqlHelper_.executeQuery(
                sql,
                {
                    SqlValue::fromString(value)
                });

        std::vector<T> result;

        for (const auto& row : rows)
        {
            result.push_back(
                SqlTraits<T>::fromRow(row));
        }

        return result;
    }

    void insert(const T& entity)
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildInsertSql(metadata);

        const auto parameters =
            SqlTraits<T>::toParameters(entity);

        sqlHelper_.executeNonQuery(
            sql,
            parameters);
    }

    std::vector<T> selectAll()
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildSelectSql(metadata);

        const auto rows =
            sqlHelper_.executeQuery(
                sql,
                {});

        std::vector<T> result;

        result.reserve(rows.size());

        for (const auto& row : rows)
        {
            result.push_back(
                SqlTraits<T>::fromRow(row));
        }

        return result;
    }

    std::vector<T> selectWhere(
        const std::string& whereClause,
        const SqlParameters& parameters)
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildSelectSql(
                metadata,
                whereClause);

        const auto rows =
            sqlHelper_.executeQuery(
                sql,
                parameters);

        std::vector<T> result;

        result.reserve(rows.size());

        for (const auto& row : rows)
        {
            result.push_back(
                SqlTraits<T>::fromRow(row));
        }

        return result;
    }

    void update(
        const T& entity,
        const std::string& whereClause)
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildUpdateSql(
                metadata,
                whereClause);

        const auto parameters =
            SqlTraits<T>::toParameters(entity);

        sqlHelper_.executeNonQuery(
            sql,
            parameters);
    }

    void remove(
        const std::string& whereClause,
        const SqlParameters& parameters)
    {
        const auto metadata =
            SqlTraits<T>::metadata();

        const auto sql =
            sqlHelper_.buildDeleteSql(
                metadata,
                whereClause);

        sqlHelper_.executeNonQuery(
            sql,
            parameters);
    }

private:

    ISqlHelper& sqlHelper_;
};

}