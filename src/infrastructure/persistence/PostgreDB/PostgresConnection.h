#pragma once

#include "../IDatabaseConnection.h"

namespace infrastructure::persistence::postgresql
{

class PostgresConnection
    : public infrastructure::persistence::IDatabaseConnection
{
public:
    PostgresConnection() = default;

    ~PostgresConnection() override;

    // Non-copyable
    PostgresConnection(const PostgresConnection&) = delete;
    PostgresConnection& operator=(
        const PostgresConnection&) = delete;

    // Movable
    PostgresConnection(PostgresConnection&& other) noexcept;
    PostgresConnection& operator=(
        PostgresConnection&& other) noexcept;

    // ========================================================
    // Connection
    // ========================================================

    SQLHDBC connect(
        const std::string& connectionString) override;

    void disconnect() override;

    // ========================================================
    // Statement lifecycle
    // ========================================================

    SQLHSTMT allocateStatement(
        SQLHDBC hDbc) override;

    void freeStatement(
        SQLHSTMT stmt) override;

    // ========================================================
    // Statement execution
    // ========================================================

    void prepareStatement(
        SQLHSTMT stmt,
        const std::string& sql) override;

    void executeStatement(
        SQLHSTMT stmt) override;

    // ========================================================
    // Fetch
    // ========================================================

    SQLRETURN fetch(
        SQLHSTMT stmt) override;

private:

    SQLHENV environment_ = SQL_NULL_HENV;
    SQLHDBC connection_ = SQL_NULL_HDBC;
};

}