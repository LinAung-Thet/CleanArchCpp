#pragma once
#include "../../src/domain/repositories/IUserRepository.h"

class MockUserRepository : public domain::repositories::IUserRepository {
public:
    std::optional<domain::entities::User> storedUser;

    void add(const domain::entities::User& user) override {
        storedUser = user;
    }

    std::optional<domain::entities::User>
    findByEmail(const std::string& email) const override {
        if (storedUser && storedUser->email().value() == email)
            return storedUser;
        return std::nullopt;
    }
};