#pragma once
#include <optional>
#include <string>
#include "../entities/User.h"

namespace domain::repositories {

class IUserRepository {
public:
    virtual ~IUserRepository() = default;

    virtual void add(const entities::User& user) = 0;
    virtual std::optional<entities::User> findByEmail(const std::string& email) = 0;
};

} // namespace domain::repositories
