#include <iostream>
#include <string>
#include <stdexcept>

#include "InDbUserRepository.h"

namespace infrastructure::persistence {

InDbUserRepository::InDbUserRepository(const std::string& connectionString, 
                                       IDatabaseConnection& db,
                                       ISqlHelper& serverHelper,
                                       DbAdapter<domain::entities::User>& dbAdapter)
    : connectionString_(connectionString), db_(db), serverHelper_(serverHelper), dbAdapter_(dbAdapter) {}

InDbUserRepository::~InDbUserRepository() {}

void InDbUserRepository::add(const domain::entities::User& user) {
    dbAdapter_.add(user);
}

std::optional<domain::entities::User> InDbUserRepository::findByEmail(const std::string& email) {
    auto users = dbAdapter_.findByColumn("Email", email);

    if (!users.empty())
        return users[0];

    return std::nullopt;
}

} // namespace infrastructure::persistence::sqlserver
