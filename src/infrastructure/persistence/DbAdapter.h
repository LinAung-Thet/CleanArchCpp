#pragma once

#include <Windows.h>
#include <string>
#include <stdexcept>
#include <sql.h>
#include <sqlext.h>

#include "sqlserver/SqlServerHelper.h"
#include "sqlserver/SqlServerRepository.h"

namespace infrastructure::persistence {

template <typename T>
class DbAdapter {
public:
    explicit DbAdapter(sqlserver::SqlServerRepository& repo, std::string connectionString)
        : connectionString_(std::move(connectionString)), sqlServerRepo_(repo) {}

    void add(const T& entity) {
        SQLHDBC hDbc = sqlServerRepo_.connect(connectionString_);

        SQLHSTMT stmt = nullptr;

        SQLRETURN ret = SQLAllocHandle(
            SQL_HANDLE_STMT,
            hDbc,
            &stmt
        );

        if (!SQL_SUCCEEDED(ret)) {
            throw std::runtime_error("Failed to allocate statement");
        }

        std::string sql =
            infrastructure::persistence::sqlserver::
            SqlServerHelper::buildInsertSql<T>();

        ret = SQLPrepare(
            stmt,
            reinterpret_cast<SQLCHAR*>(sql.data()),
            SQL_NTS
        );

        if (!SQL_SUCCEEDED(ret)) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            throw std::runtime_error("Failed to prepare SQL");
        }

        // These MUST remain alive until SQLExecute() finishes.
        std::vector<std::string> buffers;
        std::vector<SQLLEN> lengths;

        sqlserver::SqlServerHelper::bindParameters(
            stmt,
            entity,
            buffers,
            lengths
        );

        ret = SQLExecute(stmt);

        std::cout << "SQLExecute returned: " << ret << '\n';

        if (ret == SQL_ERROR) {
            SQLCHAR sqlState[6] = {};
            SQLCHAR message[SQL_MAX_MESSAGE_LENGTH] = {};
            SQLINTEGER nativeError = 0;
            SQLSMALLINT messageLength = 0;

            SQLRETURN diagRet = SQLGetDiagRec(
                SQL_HANDLE_STMT,
                stmt,
                1,
                sqlState,
                &nativeError,
                message,
                sizeof(message),
                &messageLength
            );

            std::cout << "SQLGetDiagRec returned: " << diagRet << '\n';
            std::cout << "SQL State: " << sqlState << '\n';
            std::cout << "Native error: " << nativeError << '\n';
            std::cout << "Message: " << message << '\n';
        }
        if (!SQL_SUCCEEDED(ret)) {
            sqlServerRepo_.disconnect();
            throw std::runtime_error("Failed to insert entity");
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        sqlServerRepo_.disconnect();
    }

    template <typename T> std::optional<T> findByColumn( const std::string& column, const std::string& value ) const {
        SQLHDBC hDbc = sqlServerRepo_.connect(connectionString_);

        SQLHSTMT stmt;
        SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &stmt);

        std::string sql = infrastructure::persistence::sqlserver::SqlServerHelper::buildSelectByColumnSql<T>(column);
        SQLPrepare(stmt, (SQLCHAR*)sql.c_str(), SQL_NTS);

        SQLLEN len = (SQLLEN)value.size();

        SQLBindParameter(
            stmt,
            1,
            SQL_PARAM_INPUT,
            SQL_C_CHAR,
            SQL_VARCHAR,
            value.size(),
            0,
            (SQLPOINTER)value.c_str(),
            value.size(),   // buffer length
            &len
        );

        auto ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        // Allocate buffers for each column
        std::vector<std::vector<char>> buffers(infrastructure::persistence::sqlserver::SqlTraits<T>::columns.size());
        infrastructure::persistence::sqlserver::SqlServerHelper::bindOutputColumns<T>(stmt, buffers);

        if (SQLFetch(stmt) == SQL_NO_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return std::nullopt;
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);

        sqlServerRepo_.disconnect();

        // Construct entity using SqlTraits<T>
        return infrastructure::persistence::sqlserver::SqlTraits<T>::fromRow(
            buffers[0].data(),
            buffers[1].data(),
            buffers[2].data()
        );
    }

private:
    std::string connectionString_;
    sqlserver::SqlServerRepository& sqlServerRepo_;
};

} // namespace infrastructure::persistence
