#pragma once
#include "../IDatabaseConnection.h"

namespace infrastructure::persistence::sqlserver {

class SqlServerConnection : public IDatabaseConnection {
public:
    SqlServerConnection(const std::string& connectionString) {
        connect(connectionString);
    }
    ~SqlServerConnection() override = default;

    SQLHDBC connect(const std::string& connectionString) override {
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &hEnv_);
        SQLSetEnvAttr(hEnv_, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

        SQLAllocHandle(SQL_HANDLE_DBC, hEnv_, &hDbc_);

        SQLCHAR outConnStr[1024];
        SQLSMALLINT outConnStrLen;

        auto ret = SQLDriverConnect(
            hDbc_,
            nullptr,
            (SQLCHAR*)connectionString.c_str(),
            SQL_NTS,
            outConnStr,
            sizeof(outConnStr),
            &outConnStrLen,
            SQL_DRIVER_NOPROMPT
        );

        if (!SQL_SUCCEEDED(ret)) {
            SQLCHAR sqlState[6], message[1024];
            SQLINTEGER nativeError;
            SQLSMALLINT messageLen;

            SQLGetDiagRec(SQL_HANDLE_DBC, hDbc_, 1, sqlState, &nativeError,
                        message, sizeof(message), &messageLen);

            throw std::runtime_error("Failed to connect to SQL Server");
        }
        return hDbc_;
    }

    void disconnect() override {
        if (hDbc_) {
            SQLDisconnect(hDbc_);
            SQLFreeHandle(SQL_HANDLE_DBC, hDbc_);
        }
        if (hEnv_) {
            SQLFreeHandle(SQL_HANDLE_ENV, hEnv_);
        }
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