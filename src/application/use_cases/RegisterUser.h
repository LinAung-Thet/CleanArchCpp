#pragma once
#include <string>

#include "../dto/RegisterUserRequest.h"
#include "../../domain/repositories/IUserRepository.h"

namespace application::use_cases {

class RegisterUser {
public:
    explicit RegisterUser(domain::repositories::IUserRepository& repo);

    std::string execute(const dto::RegisterUserRequest& request);

private:
    domain::repositories::IUserRepository& repo_;

    static std::string generateId();
};

} // namespace application::use_cases
