#include "../IDatabaseConnection.h"

namespace infrastructure::persistence::sqlserver {

class SqlServerConnection : public IDatabaseConnection {
public:
    SqlServerConnection(const std::string& connectionString) {
        connect(connectionString);
    }
    ~SqlServerConnection() override = default;

    void connect(const std::string& connectionString) override {
        // ODBC connect
    }

    void disconnect() override {
        // ODBC disconnect
    }

    void* allocateStatement() override {
        SQLHSTMT stmt;
        return &stmt;
        // return SQLHSTMT
    }

    void prepareStatement(void* stmt, const std::string& sql) override {
        // SQLPrepare
    }

    void executeStatement(void* stmt) override {
        // SQLExecute
    }
};
}