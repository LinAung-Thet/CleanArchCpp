#pragma once
#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <optional>
#include <string>

namespace infrastructure::persistence::sqlserver {

class SqlServerRepository {
public:
    SqlServerRepository();
    SqlServerRepository(const std::string& connectionString);
    ~SqlServerRepository();
    SQLHDBC connect(const std::string& connectionString);
    void disconnect();

private:
    SQLHENV hEnv_;
    SQLHDBC hDbc_;

    template <typename T> std::string buildInsertSql();
    template <typename T> void bindParameters(SQLHSTMT stmt, const T& entity);

};

} // namespace infrastructure::persistence::sqlserver
