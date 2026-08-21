#pragma once

#include "../IDatabaseConnection.h"

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

#include <stdexcept>
#include <string>

namespace infrastructure::persistence::sqlserver
{
class SqlServer final
    : public IDatabaseConnection
{
private:
    // ODBC handles
    SQLHENV hEnv_ = SQL_NULL_HENV;
    SQLHDBC hDbc_ = SQL_NULL_HDBC;

public:

    SqlServer() = default;

    ~SqlServer() override
    {
        disconnect();
    }

    SqlServer(
        const SqlServer&) = delete;

    SqlServer& operator=(
        const SqlServer&) = delete;

    // ========================================================
    // Connection
    // ========================================================

    SQLHDBC connect(
        const std::string& connectionString) override
    {
        if (hDbc_ != SQL_NULL_HDBC)
        {
            return hDbc_;
        }

        SQLRETURN ret =
            SQLAllocHandle(
                SQL_HANDLE_ENV,
                SQL_NULL_HANDLE,
                &hEnv_);

        if (!SQL_SUCCEEDED(ret))
        {
            hEnv_ = SQL_NULL_HENV;
            return SQL_NULL_HDBC;
        }

        ret =
            SQLSetEnvAttr(
                hEnv_,
                SQL_ATTR_ODBC_VERSION,
                reinterpret_cast<
                    SQLPOINTER>(
                        SQL_OV_ODBC3),
                0);

        if (!SQL_SUCCEEDED(ret))
        {
            cleanupEnvironment();
            return SQL_NULL_HDBC;
        }

        ret =
            SQLAllocHandle(
                SQL_HANDLE_DBC,
                hEnv_,
                &hDbc_);

        if (!SQL_SUCCEEDED(ret))
        {
            cleanupEnvironment();
            return SQL_NULL_HDBC;
        }

        ret =
            SQLDriverConnectA(
                hDbc_,
                nullptr,
                reinterpret_cast<
                    SQLCHAR*>(
                        const_cast<char*>(
                            connectionString.c_str())),
                SQL_NTS,
                nullptr,
                0,
                nullptr,
                SQL_DRIVER_NOPROMPT);

        if (!SQL_SUCCEEDED(ret))
        {
            SQLFreeHandle(
                SQL_HANDLE_DBC,
                hDbc_);

            hDbc_ = SQL_NULL_HDBC;

            cleanupEnvironment();

            return SQL_NULL_HDBC;
        }

        return hDbc_;
    }

    // ========================================================
    // Disconnect
    // ========================================================

    void disconnect() override
    {
        if (hDbc_ != SQL_NULL_HDBC)
        {
            SQLDisconnect(hDbc_);

            SQLFreeHandle(
                SQL_HANDLE_DBC,
                hDbc_);

            hDbc_ = SQL_NULL_HDBC;
        }

        cleanupEnvironment();
    }

    // ========================================================
    // Allocate statement
    // ========================================================

    SQLHSTMT allocateStatement(
        SQLHDBC hDbc) override
    {
        if (hDbc == SQL_NULL_HDBC)
        {
            throw std::runtime_error(
                "Invalid database connection");
        }

        SQLHSTMT stmt =
            SQL_NULL_HSTMT;

        SQLRETURN ret =
            SQLAllocHandle(
                SQL_HANDLE_STMT,
                hDbc,
                &stmt);

        if (!SQL_SUCCEEDED(ret))
        {
            throw std::runtime_error(
                "SQLAllocHandle(STMT) failed");
        }

        return stmt;
    }

    // ========================================================
    // Free statement
    // ========================================================

    void freeStatement(
        SQLHSTMT stmt) override
    {
        if (stmt != SQL_NULL_HSTMT)
        {
            SQLFreeHandle(
                SQL_HANDLE_STMT,
                stmt);
        }
    }

    // ========================================================
    // Prepare
    // ========================================================

    void prepareStatement(
        SQLHSTMT stmt,
        const std::string& sql) override
    {
        SQLRETURN ret =
            SQLPrepareA(
                stmt,
                reinterpret_cast<
                    SQLCHAR*>(
                        const_cast<char*>(
                            sql.c_str())),
                SQL_NTS);

        if (!SQL_SUCCEEDED(ret))
        {
            throw std::runtime_error(
                "SQLPrepare failed");
        }
    }

    // ========================================================
    // Execute
    // ========================================================

    void executeStatement(
        SQLHSTMT stmt) override
    {
        SQLRETURN ret =
            SQLExecute(stmt);

        if (!SQL_SUCCEEDED(ret))
        {
            throw std::runtime_error(
                "SQLExecute failed");
        }
    }

    SQLRETURN SqlServer::fetch(
    SQLHSTMT stmt) override
    {
        return SQLFetch(stmt);
    }
private:

    void cleanupEnvironment()
    {
        if (hEnv_ != SQL_NULL_HENV)
        {
            SQLFreeHandle(
                SQL_HANDLE_ENV,
                hEnv_);

            hEnv_ = SQL_NULL_HENV;
        }
    }
};

}