#include <iostream>
#include <string>
#include <stdexcept>

#include "UserRepositoryInDb.h"
#include "UserSqlTraits.h"  // required to instantiate DbAdapter<domain::entities::User>

namespace infrastructure::persistence {

UserRepositoryInDb::UserRepositoryInDb(DbAdapter<domain::entities::User>& dbAdapter)
    : dbAdapter_(dbAdapter) {}

UserRepositoryInDb::~UserRepositoryInDb() {}

void UserRepositoryInDb::add(const domain::entities::User& user) {
    dbAdapter_.add(user);
}

std::optional<domain::entities::User> UserRepositoryInDb::findByEmail(const std::string& email) {
    auto users = dbAdapter_.findByColumn("Email", email);

    if (!users.empty())
        return users[0];

    return std::nullopt;
}

} // namespace infrastructure::persistence::sqlserver
