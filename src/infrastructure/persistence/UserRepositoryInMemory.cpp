#include "UserRepositoryInMemory.h"

namespace infrastructure::persistence {

void UserRepositoryInMemory::add(const domain::entities::User& user) {
    users_.push_back(user);
}

std::optional<domain::entities::User>
UserRepositoryInMemory::findByEmail(const std::string& email) {
    for (const auto& u : users_) {
        if (u.email().value() == email) {
            return u;
        }
    }
    return std::nullopt;
}

} // namespace infrastructure::persistence
