#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#include <iostream>
#include <string>
#include <stdexcept>

#include "InDbUserRepository.h"

namespace infrastructure::persistence {

InDbUserRepository::InDbUserRepository(IDatabaseConnection& db, const std::string& connectionString)
    : connectionString_(connectionString),
      db_(db),
    //   sqlServerRepo_(connectionString_),            // construct the real object
      dbRepo_(connectionString_, db_)    // pass it BY REFERENCE
{}

InDbUserRepository::~InDbUserRepository() {}

void InDbUserRepository::add(const domain::entities::User& user) {
    dbRepo_.add(user);
}

std::optional<domain::entities::User> InDbUserRepository::findByEmail(const std::string& email) {
    return dbRepo_.findByColumn<domain::entities::User>("Email", email);
}

} // namespace infrastructure::persistence::sqlserver
