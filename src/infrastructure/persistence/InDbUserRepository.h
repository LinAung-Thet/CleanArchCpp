#pragma once
#undef UNICODE
#undef _UNICODE
#include <optional>
#include <string>
#include "../../domain/repositories/IUserRepository.h"
#include "DbAdapter.h"
#include "IDatabaseConnection.h"

namespace infrastructure::persistence {

class InDbUserRepository : public domain::repositories::IUserRepository {
public:
    InDbUserRepository(const std::string& connectionString, 
        IDatabaseConnection& db, IServerHelper& serverHelper, DbAdapter<domain::entities::User>& dbRepo);
    ~InDbUserRepository();

    void add(const domain::entities::User& user) override;
    std::optional<domain::entities::User> 
    findByEmail(const std::string& email) override;

private:
    const std::string& connectionString_;
    IDatabaseConnection& db_;
    IServerHelper& serverHelper_;
    DbAdapter<domain::entities::User>& dbAdapter_;   // REFERENCE inside DbAdapter

};

} // namespace infrastructure::persistence::sqlserver
