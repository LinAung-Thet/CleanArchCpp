#pragma once

#include "../IServerHelper.h"
#include "../SqlTraits.h"

#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

class SqlServerHelper final
    : public IServerHelper
{
public:

    // ========================================================
    // Register any entity
    // ========================================================

    template<typename T>
    void registerType()
    {
        registry_[typeid(T).hash_code()] =
        {
            // INSERT
            [](const void* object)
            {
                const T& entity =
                    *static_cast<const T*>(object);

                return buildInsertSqlImpl<T>(entity);
            },

            // BIND PARAMETERS
            [](SQLHSTMT stmt,
               const void* object,
               std::vector<std::string>& buffers,
               std::vector<SQLLEN>& lengths)
            {
                const T& entity =
                    *static_cast<const T*>(object);

                bindParametersImpl<T>(
                    stmt,
                    entity,
                    buffers,
                    lengths);
            },

            // SELECT
            [](const std::string& column)
            {
                return buildSelectSqlImpl<T>(column);
            },

            // OUTPUT
            [](SQLHSTMT stmt,
               std::vector<std::vector<char>>& buffers)
            {
                bindOutputColumnsImpl<T>(
                    stmt,
                    buffers);
            },

            // COLUMN COUNT
            []()
            {
                return infrastructure::persistence::
                    SqlTraits<T>::columnCount();
            }
        };
    }

protected:

    // ========================================================
    // Column count
    // ========================================================

    std::size_t columnCountImpl(
        const std::type_info& type) const override
    {
        return columnCountFromRegistry(type);
    }

private:

    // ========================================================
    // SELECT
    // ========================================================

    template<typename T>
    static std::string buildSelectSqlImpl(
        const std::string& column)
    {
        using Traits =
            infrastructure::persistence::SqlTraits<T>;

        std::ostringstream sql;

        sql << "SELECT ";

        for (std::size_t i = 0;
             i < Traits::columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql << ", ";
            }

            sql << quoteIdentifier(
                Traits::columns[i]);
        }

        sql << " FROM "
            << quoteIdentifier(
                Traits::table);

        sql << " WHERE "
            << quoteIdentifier(column)
            << " = ?";

        return sql.str();
    }

    // ========================================================
    // INSERT
    // ========================================================

    template<typename T>
    static std::string buildInsertSqlImpl(
        const T& /*entity*/)
    {
        using Traits =
            infrastructure::persistence::SqlTraits<T>;

        std::ostringstream sql;

        sql << "INSERT INTO "
            << quoteIdentifier(Traits::table)
            << " (";

        for (std::size_t i = 0;
             i < Traits::columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql << ", ";
            }

            sql << quoteIdentifier(
                Traits::columns[i]);
        }

        sql << ") VALUES (";

        for (std::size_t i = 0;
             i < Traits::columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql << ", ";
            }

            sql << "?";
        }

        sql << ")";

        return sql.str();
    }

    // ========================================================
    // Bind input parameters
    // ========================================================

    template<typename T>
    static void bindParametersImpl(
        SQLHSTMT stmt,
        const T& entity,
        std::vector<std::string>& buffers,
        std::vector<SQLLEN>& lengths)
    {
        using Traits =
            infrastructure::persistence::SqlTraits<T>;

        const std::size_t count =
            Traits::columnCount();

        buffers.resize(count);
        lengths.resize(count);

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            buffers[i] =
                Traits::getField(entity, i);

            lengths[i] =
                static_cast<SQLLEN>(
                    buffers[i].size());

            SQLRETURN ret =
                SQLBindParameter(
                    stmt,
                    static_cast<SQLUSMALLINT>(i + 1),
                    SQL_PARAM_INPUT,
                    SQL_C_CHAR,
                    SQL_VARCHAR,
                    Traits::columnSizes[i],
                    0,
                    buffers[i].data(),
                    buffers[i].size(),
                    &lengths[i]);

            if (!SQL_SUCCEEDED(ret))
            {
                throw std::runtime_error(
                    "SQLBindParameter failed");
            }
        }
    }

    // ========================================================
    // Bind output columns
    // ========================================================

    template<typename T>
    static void bindOutputColumnsImpl(
        SQLHSTMT stmt,
        std::vector<std::vector<char>>& buffers)
    {
        using Traits =
            infrastructure::persistence::SqlTraits<T>;

        const std::size_t count =
            Traits::columnCount();

        buffers.resize(count);

        for (std::size_t i = 0;
             i < count;
             ++i)
        {
            // +1 for '\0'
            buffers[i].resize(
                Traits::columnSizes[i] + 1);

            SQLRETURN ret =
                SQLBindCol(
                    stmt,
                    static_cast<SQLUSMALLINT>(i + 1),
                    SQL_C_CHAR,
                    buffers[i].data(),
                    buffers[i].size(),
                    nullptr);

            if (!SQL_SUCCEEDED(ret))
            {
                throw std::runtime_error(
                    "SQLBindCol failed");
            }
        }
    }

    // ========================================================
    // Registry column count
    // ========================================================

    std::size_t columnCountFromRegistry(
        const std::type_info& type) const
    {
        const auto it =
            registry_.find(type.hash_code());

        if (it == registry_.end())
        {
            throw std::runtime_error(
                "Type is not registered");
        }

        return it->second.columnCountFn();
    }

    // ========================================================
    // SQL Server identifier quoting
    // ========================================================

    static std::string quoteIdentifier(
        std::string_view identifier)
    {
        return "[" +
               std::string(identifier) +
               "]";
    }
};