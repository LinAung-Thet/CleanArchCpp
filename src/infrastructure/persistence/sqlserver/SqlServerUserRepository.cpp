#include "SqlServerUserRepository.h"
#include <iostream>

namespace infrastructure::persistence::sqlserver {

SqlServerUserRepository::SqlServerUserRepository(const std::string& connectionString)
    : hEnv_(nullptr), hDbc_(nullptr)
{
    connect(connectionString);
}

SqlServerUserRepository::~SqlServerUserRepository() {
    disconnect();
}

void SqlServerUserRepository::connect(const std::string& connectionString) {
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

        std::cout << "State: " << sqlState << "\n"
                << "Native: " << nativeError << "\n"
                << "Message: " << message << "\n";

        throw std::runtime_error("Failed to connect to SQL Server");
    }
}

void SqlServerUserRepository::disconnect() {
    if (hDbc_) {
        SQLDisconnect(hDbc_);
        SQLFreeHandle(SQL_HANDLE_DBC, hDbc_);
    }
    if (hEnv_) {
        SQLFreeHandle(SQL_HANDLE_ENV, hEnv_);
    }
}

void SqlServerUserRepository::add(const domain::entities::User& user) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc_, &stmt);

    const char* sql =
        "INSERT INTO Users (Id, Name, Email) VALUES (?, ?, ?)";

    SQLPrepare(stmt, (SQLCHAR*)sql, SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 50, 0,
                     (SQLPOINTER)user.id().c_str(), 0, nullptr);

    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 100, 0,
                     (SQLPOINTER)user.name().c_str(), 0, nullptr);

    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 200, 0,
                     (SQLPOINTER)user.email().value().c_str(), 0, nullptr);

    auto ret = SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (!SQL_SUCCEEDED(ret)) {
        throw std::runtime_error("Failed to insert user");
    }
}

std::optional<domain::entities::User>
SqlServerUserRepository::findByEmail(const std::string& email) const {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc_, &stmt);

    const char* sql =
        "SELECT Id, Name, Email FROM Users WHERE Email = ?";

    SQLPrepare(stmt, (SQLCHAR*)sql, SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_CHAR, 200, 0,
                     (SQLPOINTER)email.c_str(), 0, nullptr);

    auto ret = SQLExecute(stmt);
    if (!SQL_SUCCEEDED(ret)) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    char id[50], name[100], emailBuf[200];
    SQLBindCol(stmt, 1, SQL_C_CHAR, id, sizeof(id), nullptr);
    SQLBindCol(stmt, 2, SQL_C_CHAR, name, sizeof(name), nullptr);
    SQLBindCol(stmt, 3, SQL_C_CHAR, emailBuf, sizeof(emailBuf), nullptr);

    if (SQLFetch(stmt) == SQL_NO_DATA) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return std::nullopt;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return domain::entities::User{
        id,
        name,
        domain::value_objects::Email(emailBuf)
    };
}

} // namespace infrastructure::persistence::sqlserver
