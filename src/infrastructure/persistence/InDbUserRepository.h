#pragma once
#undef UNICODE
#undef _UNICODE
#include <optional>
#include <string>
#include "../../domain/repositories/IUserRepository.h"
#include "DbAdapter.h"

namespace infrastructure::persistence {

class InDbUserRepository : public domain::repositories::IUserRepository {
public:
    InDbUserRepository(DbAdapter<domain::entities::User>& dbRepo);
    ~InDbUserRepository();

    void add(const domain::entities::User& user) override;
    std::optional<domain::entities::User> 
    findByEmail(const std::string& email) override;

private:
    DbAdapter<domain::entities::User>& dbAdapter_;   // REFERENCE inside DbAdapter

};

} // namespace infrastructure::persistence::sqlserver
