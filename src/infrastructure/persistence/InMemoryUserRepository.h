#pragma once
#include <vector>
#include <optional>
#include <string>

#include "../../domain/repositories/IUserRepository.h"

namespace infrastructure::persistence {

class InMemoryUserRepository : public domain::repositories::IUserRepository {
public:
    void add(const domain::entities::User& user) override;

    std::optional<domain::entities::User>
    findByEmail(const std::string& email) const override;

private:
    std::vector<domain::entities::User> users_;
};

} // namespace infrastructure::persistence
