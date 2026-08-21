#pragma once

#include "IDatabaseConnection.h"
#include "IServerHelper.h"
#include "SqlTraits.h"

#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
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
    // Add
    // ========================================================

    bool add(
        const T& entity) const
    {
        SQLHDBC hDbc =
            db_.connect(connectionString_);

        if (hDbc == SQL_NULL_HDBC)
        {
            return false;
        }

        SQLHSTMT stmt =
            SQL_NULL_HSTMT;

        try
        {
            // ------------------------------------------------
            // Allocate statement
            // ------------------------------------------------

            stmt =
                db_.allocateStatement(hDbc);

            // ------------------------------------------------
            // Generate INSERT SQL
            // ------------------------------------------------

            const std::string sql =
                serverHelper_.buildInsertSql<T>(
                    entity);

            // ------------------------------------------------
            // Prepare
            // ------------------------------------------------

            db_.prepareStatement(
                stmt,
                sql);

            // ------------------------------------------------
            // Bind parameters
            // ------------------------------------------------

            std::vector<std::string> buffers;
            std::vector<SQLLEN> lengths;

            serverHelper_.bindParameters<T>(
                stmt,
                entity,
                buffers,
                lengths);

            // ------------------------------------------------
            // Execute
            // ------------------------------------------------

            db_.executeStatement(stmt);

            // ------------------------------------------------
            // Cleanup
            // ------------------------------------------------

            db_.freeStatement(stmt);

            db_.disconnect();

            return true;
        }
        catch (...)
        {
            if (stmt != SQL_NULL_HSTMT)
            {
                db_.freeStatement(stmt);
            }

            db_.disconnect();

            return false;
        }
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

        if (hDbc == SQL_NULL_HDBC)
        {
            return std::nullopt;
        }

        SQLHSTMT stmt =
            SQL_NULL_HSTMT;

        try
        {
            // ------------------------------------------------
            // Allocate statement
            // ------------------------------------------------

            stmt =
                db_.allocateStatement(hDbc);

            // ------------------------------------------------
            // Generate SELECT SQL
            // ------------------------------------------------

            const std::string sql =
                serverHelper_.buildSelectByColumnSql<T>(
                    column);

            // ------------------------------------------------
            // Prepare
            // ------------------------------------------------

            db_.prepareStatement(
                stmt,
                sql);

            // ------------------------------------------------
            // Bind WHERE parameter
            // ------------------------------------------------

            std::vector<std::string> parameterBuffers;
            std::vector<SQLLEN> parameterLengths;

            parameterBuffers.push_back(value);

            parameterLengths.push_back(
                static_cast<SQLLEN>(
                    parameterBuffers[0].size()));

            SQLRETURN ret =
                SQLBindParameter(
                    stmt,
                    1,
                    SQL_PARAM_INPUT,
                    SQL_C_CHAR,
                    SQL_VARCHAR,
                    parameterBuffers[0].size(),
                    0,
                    parameterBuffers[0].data(),
                    parameterBuffers[0].size(),
                    &parameterLengths[0]);

            if (!SQL_SUCCEEDED(ret))
            {
                db_.freeStatement(stmt);
                db_.disconnect();

                return std::nullopt;
            }

            // ------------------------------------------------
            // Execute
            // ------------------------------------------------

            db_.executeStatement(stmt);

            // ------------------------------------------------
            // Allocate output buffers
            // ------------------------------------------------

            std::vector<std::vector<char>> buffers(
                serverHelper_.columnCount<T>());

            // ------------------------------------------------
            // Bind output columns
            // ------------------------------------------------

            serverHelper_.bindOutputColumns<T>(
                stmt,
                buffers);

            // ------------------------------------------------
            // Fetch
            // ------------------------------------------------

            ret =
                db_.fetch(stmt);

            if (ret == SQL_NO_DATA)
            {
                db_.freeStatement(stmt);
                db_.disconnect();

                return std::nullopt;
            }

            if (!SQL_SUCCEEDED(ret))
            {
                db_.freeStatement(stmt);
                db_.disconnect();

                return std::nullopt;
            }

            // ------------------------------------------------
            // Cleanup
            // ------------------------------------------------

            db_.freeStatement(stmt);

            db_.disconnect();

            // ------------------------------------------------
            // Convert row -> entity
            // ------------------------------------------------

            return SqlTraits<T>::fromRow(
                buffers);
        }
        catch (...)
        {
            if (stmt != SQL_NULL_HSTMT)
            {
                db_.freeStatement(stmt);
            }

            db_.disconnect();

            return std::nullopt;
        }
    }

private:

    std::string connectionString_;

    IDatabaseConnection& db_;

    IServerHelper& serverHelper_;
};

}