#include "PostgresConnection.h"

#include <stdexcept>
#include <utility>

namespace infrastructure::persistence::postgresql
{

PostgresConnection::~PostgresConnection()
{
    disconnect();
}

PostgresConnection::PostgresConnection(
    PostgresConnection&& other) noexcept
    : environment_(other.environment_),
      connection_(other.connection_)
{
    other.environment_ = SQL_NULL_HENV;
    other.connection_ = SQL_NULL_HDBC;
}

PostgresConnection&
PostgresConnection::operator=(
    PostgresConnection&& other) noexcept
{
    if (this != &other)
    {
        disconnect();

        environment_ = other.environment_;
        connection_ = other.connection_;

        other.environment_ = SQL_NULL_HENV;
        other.connection_ = SQL_NULL_HDBC;
    }

    return *this;
}

SQLHDBC PostgresConnection::connect(
    const std::string& connectionString)
{
    // Close an existing connection first.
    disconnect();

    SQLRETURN result;

    // ========================================================
    // Allocate ODBC environment
    // ========================================================

    result = SQLAllocHandle(
        SQL_HANDLE_ENV,
        SQL_NULL_HANDLE,
        &environment_);

    if (!SQL_SUCCEEDED(result))
    {
        environment_ = SQL_NULL_HENV;

        throw std::runtime_error(
            "Failed to allocate ODBC environment.");
    }

    // ========================================================
    // Set ODBC version
    // ========================================================

    result = SQLSetEnvAttr(
        environment_,
        SQL_ATTR_ODBC_VERSION,
        reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3),
        0);

    if (!SQL_SUCCEEDED(result))
    {
        disconnect();

        throw std::runtime_error(
            "Failed to configure ODBC version.");
    }

    // ========================================================
    // Allocate connection handle
    // ========================================================

    result = SQLAllocHandle(
        SQL_HANDLE_DBC,
        environment_,
        &connection_);

    if (!SQL_SUCCEEDED(result))
    {
        connection_ = SQL_NULL_HDBC;

        disconnect();

        throw std::runtime_error(
            "Failed to allocate ODBC connection handle.");
    }

    // ========================================================
    // Connect
    // ========================================================

    result = SQLDriverConnectA(
        connection_,
        nullptr,
        reinterpret_cast<SQLCHAR*>(
            const_cast<char*>(connectionString.c_str())),
        SQL_NTS,
        nullptr,
        0,
        nullptr,
        SQL_DRIVER_NOPROMPT);

    if (!SQL_SUCCEEDED(result))
    {
        disconnect();

        throw std::runtime_error(
            "Failed to connect to PostgreSQL through ODBC.");
    }

    return connection_;
}

void PostgresConnection::disconnect()
{
    if (connection_ != SQL_NULL_HDBC)
    {
        SQLDisconnect(connection_);

        SQLFreeHandle(
            SQL_HANDLE_DBC,
            connection_);

        connection_ = SQL_NULL_HDBC;
    }

    if (environment_ != SQL_NULL_HENV)
    {
        SQLFreeHandle(
            SQL_HANDLE_ENV,
            environment_);

        environment_ = SQL_NULL_HENV;
    }
}

SQLHSTMT PostgresConnection::allocateStatement(
    SQLHDBC hDbc)
{
    if (hDbc == SQL_NULL_HDBC)
    {
        throw std::runtime_error(
            "Cannot allocate statement: invalid database connection.");
    }

    SQLHSTMT stmt = SQL_NULL_HSTMT;

    const SQLRETURN result = SQLAllocHandle(
        SQL_HANDLE_STMT,
        hDbc,
        &stmt);

    if (!SQL_SUCCEEDED(result))
    {
        throw std::runtime_error(
            "Failed to allocate SQL statement.");
    }

    return stmt;
}

void PostgresConnection::freeStatement(
    SQLHSTMT stmt)
{
    if (stmt != SQL_NULL_HSTMT)
    {
        SQLFreeHandle(
            SQL_HANDLE_STMT,
            stmt);
    }
}

void PostgresConnection::prepareStatement(
    SQLHSTMT stmt,
    const std::string& sql)
{
    if (stmt == SQL_NULL_HSTMT)
    {
        throw std::runtime_error(
            "Cannot prepare statement: invalid statement handle.");
    }

    const SQLRETURN result = SQLPrepareA(
        stmt,
        reinterpret_cast<SQLCHAR*>(
            const_cast<char*>(sql.c_str())),
        SQL_NTS);

    if (!SQL_SUCCEEDED(result))
    {
        throw std::runtime_error(
            "Failed to prepare SQL statement.");
    }
}

void PostgresConnection::executeStatement(
    SQLHSTMT stmt)
{
    if (stmt == SQL_NULL_HSTMT)
    {
        throw std::runtime_error(
            "Cannot execute statement: invalid statement handle.");
    }

    const SQLRETURN result = SQLExecute(stmt);

    if (!SQL_SUCCEEDED(result))
    {
        throw std::runtime_error(
            "Failed to execute SQL statement.");
    }
}

SQLRETURN PostgresConnection::fetch(
    SQLHSTMT stmt)
{
    if (stmt == SQL_NULL_HSTMT)
    {
        throw std::runtime_error(
            "Cannot fetch: invalid statement handle.");
    }

    return SQLFetch(stmt);
}

}