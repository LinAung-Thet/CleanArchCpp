#pragma once

#include "IServerHelper.h"
#include "SqlTraits.h"
#include "IDatabaseConnection.h"

#include <optional>
#include <string>
#include <vector>

namespace infrastructure::persistence
{
template<typename T>
class DbAdapter
{
public:

    explicit DbAdapter(
        std::string connectionString,
        IDatabaseConnection& db,
        IServerHelper& helper)
        : connectionString_(
              std::move(connectionString)),
          db_(db),
          serverHelper_(helper)
    {
    }

    // ========================================================
    // Find by column
    // ========================================================

    std::optional<T> findByColumn(
        const std::string& column,
        const std::string& value) const
    {
        SQLHDBC hDbc =
            db_.connect(connectionString_);

        SQLHSTMT stmt = SQL_NULL_HSTMT;

        SQLRETURN ret =
            SQLAllocHandle(
                SQL_HANDLE_STMT,
                hDbc,
                &stmt);

        if (!SQL_SUCCEEDED(ret))
        {
            db_.disconnect();
            return std::nullopt;
        }

        // ----------------------------------------------------
        // Generate SQL
        // ----------------------------------------------------

        const std::string sql =
            serverHelper_.buildSelectByColumnSql<T>(
                column);

        ret =
            SQLPrepare(
                stmt,
                reinterpret_cast<SQLCHAR*>(
                    const_cast<char*>(
                        sql.c_str())),
                SQL_NTS);

        if (!SQL_SUCCEEDED(ret))
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return std::nullopt;
        }

        // ----------------------------------------------------
        // Bind WHERE parameter
        // ----------------------------------------------------

        SQLLEN valueLength =
            static_cast<SQLLEN>(
                value.size());

        ret =
            SQLBindParameter(
                stmt,
                1,
                SQL_PARAM_INPUT,
                SQL_C_CHAR,
                SQL_VARCHAR,
                value.size(),
                0,
                const_cast<char*>(
                    value.c_str()),
                value.size(),
                &valueLength);

        if (!SQL_SUCCEEDED(ret))
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return std::nullopt;
        }

        // ----------------------------------------------------
        // Execute
        // ----------------------------------------------------

        ret = SQLExecute(stmt);

        if (!SQL_SUCCEEDED(ret))
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return std::nullopt;
        }

        // ----------------------------------------------------
        // Allocate output buffers
        // ----------------------------------------------------

        std::vector<std::vector<char>> buffers(
            serverHelper_.columnCount<T>());

        // ----------------------------------------------------
        // Bind output columns
        // ----------------------------------------------------

        serverHelper_.bindOutputColumns<T>(
            stmt,
            buffers);

        // ----------------------------------------------------
        // Fetch
        // ----------------------------------------------------

        ret = SQLFetch(stmt);

        if (ret == SQL_NO_DATA)
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return std::nullopt;
        }

        if (!SQL_SUCCEEDED(ret))
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return std::nullopt;
        }

        // ----------------------------------------------------
        // Cleanup
        // ----------------------------------------------------

        SQLFreeHandle(
            SQL_HANDLE_STMT,
            stmt);

        db_.disconnect();

        // ----------------------------------------------------
        // Convert row -> entity
        // ----------------------------------------------------

        return infrastructure::persistence::
            SqlTraits<T>::fromRow(buffers);
    }

    bool add(const T& entity) const
    {
        SQLHDBC hDbc =
            db_.connect(connectionString_);

        if (hDbc == SQL_NULL_HDBC)
        {
            return false;
        }

        SQLHSTMT stmt = SQL_NULL_HSTMT;

        SQLRETURN ret =
            SQLAllocHandle(
                SQL_HANDLE_STMT,
                hDbc,
                &stmt);

        if (!SQL_SUCCEEDED(ret))
        {
            db_.disconnect();
            return false;
        }

        try
        {
            // ----------------------------------------------------
            // Generate INSERT SQL
            // ----------------------------------------------------

            const std::string sql =
                serverHelper_.buildInsertSql<T>(
                    entity);

            ret =
                SQLPrepare(
                    stmt,
                    reinterpret_cast<SQLCHAR*>(
                        const_cast<char*>(
                            sql.c_str())),
                    SQL_NTS);

            if (!SQL_SUCCEEDED(ret))
            {
                SQLFreeHandle(
                    SQL_HANDLE_STMT,
                    stmt);

                db_.disconnect();
                return false;
            }

            // ----------------------------------------------------
            // Bind parameters
            // ----------------------------------------------------

            std::vector<std::string> buffers;
            std::vector<SQLLEN> lengths;

            serverHelper_.bindParameters<T>(
                stmt,
                entity,
                buffers,
                lengths);

            // ----------------------------------------------------
            // Execute
            // ----------------------------------------------------

            ret = SQLExecute(stmt);

            if (!SQL_SUCCEEDED(ret))
            {
                SQLFreeHandle(
                    SQL_HANDLE_STMT,
                    stmt);

                db_.disconnect();
                return false;
            }

            // ----------------------------------------------------
            // Cleanup
            // ----------------------------------------------------

            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return true;
        }
        catch (...)
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);

            db_.disconnect();

            return false;
        }
    }
private:

    std::string connectionString_;

    IDatabaseConnection& db_;

    IServerHelper& serverHelper_;
};
}