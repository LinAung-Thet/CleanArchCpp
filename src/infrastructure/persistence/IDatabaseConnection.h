#pragma once
#include <string>

namespace infrastructure::persistence {
class IDatabaseConnection {
public:
    virtual ~IDatabaseConnection() = default;

    virtual void connect(const std::string& connectionString) = 0;
    virtual void disconnect() = 0;

    virtual void* allocateStatement() = 0;
    virtual void prepareStatement(void* stmt, const std::string& sql) = 0;
    virtual void executeStatement(void* stmt) = 0;

    // Optional: diagnostics, error handling, etc.
};
}