#pragma once

#include <windows.h>
#include <sqlext.h>

#include "../ISqlHelper.h"

namespace infrastructure::persistence::sqlserver
{

class SqlServerHelper final
    : public ISqlHelper
{
public:

    explicit SqlServerHelper(
        SQLHDBC connection)
        : connection_(connection)
    {
    }

    std::string buildInsertSql(
        const SqlMetadata& metadata) const override
    {
        std::string sql =
            "INSERT INTO " +
            metadata.tableName +
            " (";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += metadata.columns[i];
        }

        sql += ") VALUES (";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += "?";
        }

        sql += ")";

        return sql;
    }

    std::string buildSelectSql(
        const SqlMetadata& metadata,
        const std::string& whereClause = "")
        const override
    {
        std::string sql = "SELECT ";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += metadata.columns[i];
        }

        sql +=
            " FROM " +
            metadata.tableName;

        if (!whereClause.empty())
        {
            sql += " WHERE ";
            sql += whereClause;
        }

        return sql;
    }

    std::string buildUpdateSql(
        const SqlMetadata& metadata,
        const std::string& whereClause)
        const override
    {
        std::string sql =
            "UPDATE " +
            metadata.tableName +
            " SET ";

        for (std::size_t i = 0;
             i < metadata.columns.size();
             ++i)
        {
            if (i > 0)
            {
                sql += ", ";
            }

            sql += metadata.columns[i];
            sql += " = ?";
        }

        sql += " WHERE ";
        sql += whereClause;

        return sql;
    }

    std::string buildDeleteSql(
        const SqlMetadata& metadata,
        const std::string& whereClause)
        const override
    {
        return
            "DELETE FROM " +
            metadata.tableName +
            " WHERE " +
            whereClause;
    }

    void executeNonQuery(
        const std::string& sql,
        const SqlParameters& parameters)
        override
    {
        SQLHSTMT stmt = nullptr;

        check(
            SQLAllocHandle(
                SQL_HANDLE_STMT,
                connection_,
                &stmt));

        try
        {
            bindParameters(
                stmt,
                parameters);

            check(
                SQLExecDirectA(
                    stmt,
                    reinterpret_cast<SQLCHAR*>(
                        const_cast<char*>(
                            sql.c_str())),
                    SQL_NTS));

            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);
        }
        catch (...)
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            throw;
        }
    }

    SqlRows executeQuery(
        const std::string& sql,
        const SqlParameters& parameters)
        override
    {
        SQLHSTMT stmt = nullptr;

        check(
            SQLAllocHandle(
                SQL_HANDLE_STMT,
                connection_,
                &stmt));

        try
        {
            bindParameters(
                stmt,
                parameters);

            check(
                SQLExecDirectA(
                    stmt,
                    reinterpret_cast<SQLCHAR*>(
                        const_cast<char*>(
                            sql.c_str())),
                    SQL_NTS));

            const auto columnCount =
                getColumnCount(stmt);

            std::vector<std::vector<char>>
                buffers(columnCount);

            std::vector<SQLLEN>
                lengths(columnCount);

            for (auto& buffer : buffers)
            {
                buffer.resize(4096);
            }

            bindOutputColumns(
                stmt,
                buffers,
                lengths);

            SqlRows rows;

            while (
                SQLFetch(stmt) != SQL_NO_DATA)
            {
                SqlRow row;

                row.reserve(columnCount);

                for (std::size_t i = 0;
                     i < columnCount;
                     ++i)
                {
                    if (lengths[i] == SQL_NULL_DATA)
                    {
                        row.emplace_back();
                    }
                    else
                    {
                        row.emplace_back(
                            buffers[i].data(),
                            static_cast<std::size_t>(
                                lengths[i]));
                    }
                }

                rows.push_back(
                    std::move(row));
            }

            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            return rows;
        }
        catch (...)
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            throw;
        }
    }

private:

    SQLHDBC connection_;

    static void check(
        SQLRETURN result)
    {
        if (!SQL_SUCCEEDED(result))
        {
            throw std::runtime_error(
                "ODBC operation failed");
        }
    }

    static SQLSMALLINT getColumnCount(
        SQLHSTMT stmt)
    {
        SQLSMALLINT count = 0;

        check(
            SQLNumResultCols(
                stmt,
                &count));

        return count;
    }

    static void bindParameters(
        SQLHSTMT stmt,
        const SqlParameters& parameters)
    {
        /*
         * The strings must remain alive until
         * SQLExecute/SQLExecDirect has completed.
         *
         * For a production implementation,
         * use a parameter-buffer structure rather
         * than binding directly to temporary strings.
         */

        for (std::size_t i = 0;
             i < parameters.size();
             ++i)
        {
            const auto& parameter =
                parameters[i];

            if (parameter.isNull)
            {
                SQLLEN length = SQL_NULL_DATA;

                check(
                    SQLBindParameter(
                        stmt,
                        static_cast<SQLUSMALLINT>(i + 1),
                        SQL_PARAM_INPUT,
                        SQL_C_CHAR,
                        SQL_VARCHAR,
                        0,
                        0,
                        nullptr,
                        0,
                        &length));
            }
            else
            {
                check(
                    SQLBindParameter(
                        stmt,
                        static_cast<SQLUSMALLINT>(i + 1),
                        SQL_PARAM_INPUT,
                        SQL_C_CHAR,
                        SQL_VARCHAR,
                        parameter.value.size(),
                        0,
                        const_cast<char*>(
                            parameter.value.data()),
                        parameter.value.size(),
                        nullptr));
            }
        }
    }

    static void bindOutputColumns(
        SQLHSTMT stmt,
        std::vector<std::vector<char>>& buffers,
        std::vector<SQLLEN>& lengths)
    {
        for (std::size_t i = 0;
             i < buffers.size();
             ++i)
        {
            check(
                SQLBindCol(
                    stmt,
                    static_cast<SQLUSMALLINT>(i + 1),
                    SQL_C_CHAR,
                    buffers[i].data(),
                    buffers[i].size(),
                    &lengths[i]));
        }
    }
};

}