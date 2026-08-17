#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>
#include <stdexcept>

#include "InDbUserRepository.h"

namespace infrastructure::persistence {

InDbUserRepository::InDbUserRepository(const std::string& connectionString)
    : connectionString_(connectionString),
      sqlServerRepo_(connectionString_),            // construct the real object
      dbRepo_(sqlServerRepo_, connectionString_)    // pass it BY REFERENCE
{}

InDbUserRepository::~InDbUserRepository() {}

void InDbUserRepository::add(const domain::entities::User& user) {
    dbRepo_.add(user);
}

std::optional<domain::entities::User> InDbUserRepository::findByEmail(const std::string& email) {
    return dbRepo_.findByColumn<domain::entities::User>("Email", email);
}

} // namespace infrastructure::persistence::sqlserver
