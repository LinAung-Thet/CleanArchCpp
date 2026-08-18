#pragma once
#include <Windows.h>
#include <sql.h>
#include <sqlext.h>
#include <string>

namespace infrastructure::persistence {
class IDatabaseConnection {
public:
    virtual ~IDatabaseConnection() = default;

    virtual SQLHDBC connect(const std::string& connectionString) = 0;
    virtual void disconnect() = 0;

    virtual void* allocateStatement() = 0;
    virtual void prepareStatement(void* stmt, const std::string& sql) = 0;
    virtual void executeStatement(void* stmt) = 0;

    // Optional: diagnostics, error handling, etc.
protected:
        SQLHENV hEnv_;
        SQLHDBC hDbc_;
};
}