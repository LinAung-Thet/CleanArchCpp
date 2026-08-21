#pragma once

#include <Windows.h>
#include <sql.h>
#include <sqlext.h>

#include <string>

namespace infrastructure::persistence
{
    
class IDatabaseConnection
{
public:

    virtual ~IDatabaseConnection() = default;

    // ========================================================
    // Connection
    // ========================================================

    virtual SQLHDBC connect(
        const std::string& connectionString) = 0;

    virtual void disconnect() = 0;

    // ========================================================
    // Statement lifecycle
    // ========================================================

    virtual SQLHSTMT allocateStatement(
        SQLHDBC hDbc) = 0;

    virtual void freeStatement(
        SQLHSTMT stmt) = 0;

    // ========================================================
    // Statement execution
    // ========================================================

    virtual void prepareStatement(
        SQLHSTMT stmt,
        const std::string& sql) = 0;

    virtual void executeStatement(
        SQLHSTMT stmt) = 0;

    // ========================================================
    // Fetch
    // ========================================================

    virtual SQLRETURN fetch(
        SQLHSTMT stmt) = 0;
};

}