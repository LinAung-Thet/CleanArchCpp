#include "InDbUserRepository.h"
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>

namespace infrastructure::persistence {

InDbUserRepository::InDbUserRepository(const std::string& connectionString)
    : connectionString_(connectionString), sqlServerRepo_(connectionString)
{
}

InDbUserRepository::~InDbUserRepository() {
    sqlServerRepo_.disconnect();
}

void InDbUserRepository::add(const domain::entities::User& user) {
    SQLHDBC hDbc = sqlServerRepo_.connect(connectionString_);

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &stmt);

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
    sqlServerRepo_.disconnect();
}

std::optional<domain::entities::User>
InDbUserRepository::findByEmail(const std::string& email) {
    SQLHDBC hDbc = sqlServerRepo_.connect(connectionString_);

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &stmt);

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
