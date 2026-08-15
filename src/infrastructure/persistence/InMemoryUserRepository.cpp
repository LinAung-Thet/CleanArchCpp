#include "InMemoryUserRepository.h"

namespace infrastructure::persistence {

void InMemoryUserRepository::add(const domain::entities::User& user) {
    users_.push_back(user);
}

std::optional<domain::entities::User>
InMemoryUserRepository::findByEmail(const std::string& email) {
    for (const auto& u : users_) {
        if (u.email().value() == email) {
            return u;
        }
    }
    return std::nullopt;
}

} // namespace infrastructure::persistence
