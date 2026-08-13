#pragma once
#undef UNICODE
#undef _UNICODE
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <optional>
#include <string>
#include "../../../domain/repositories/IUserRepository.h"

namespace infrastructure::persistence::sqlserver {

class SqlServerUserRepository : public domain::repositories::IUserRepository {
public:
    SqlServerUserRepository(const std::string& connectionString);
    ~SqlServerUserRepository();

    void add(const domain::entities::User& user) override;
    std::optional<domain::entities::User> 
    findByEmail(const std::string& email) const override;

private:
    SQLHENV hEnv_;
    SQLHDBC hDbc_;

    void connect(const std::string& connectionString);
    void disconnect();
};

} // namespace infrastructure::persistence::sqlserver
