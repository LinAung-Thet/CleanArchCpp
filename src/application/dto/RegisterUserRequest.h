#pragma once
#include <string>

namespace application::dto {

struct RegisterUserRequest {
    std::string name;
    std::string email;
};

} // namespace application::dto
