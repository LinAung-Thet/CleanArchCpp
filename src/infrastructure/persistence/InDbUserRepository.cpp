#include <iostream>
#include <string>
#include <stdexcept>

#include "InDbUserRepository.h"

namespace infrastructure::persistence {

InDbUserRepository::InDbUserRepository(const std::string& connectionString, 
                                       IDatabaseConnection& db,
                                       IServerHelper& serverHelper,
                                       DbAdapter<domain::entities::User>& dbAdapter)
    : connectionString_(connectionString), db_(db), serverHelper_(serverHelper), dbAdapter_(dbAdapter) {}

InDbUserRepository::~InDbUserRepository() {}

void InDbUserRepository::add(const domain::entities::User& user) {
    dbAdapter_.add(user);
}

std::optional<domain::entities::User> InDbUserRepository::findByEmail(const std::string& email) {
    return dbAdapter_.findByColumn("Email", email);
}

} // namespace infrastructure::persistence::sqlserver
