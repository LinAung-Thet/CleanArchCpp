#include "RegisterUser.h"

#include "../../domain/entities/User.h"
#include "../../domain/value_objects/Email.h"

namespace application::use_cases {

RegisterUser::RegisterUser(domain::repositories::IUserRepository& repo)
    : repo_(repo) {}

std::string RegisterUser::execute(const dto::RegisterUserRequest& request) {
    auto existing = repo_.findByEmail(request.email);
    if (existing.has_value()) {
        return "User already exists";
    }

    domain::value_objects::Email email(request.email);
    domain::entities::User user(generateId(), request.name, email);
    repo_.add(user);

    return "User registered with id: " + user.id();
}

std::string RegisterUser::generateId() {
    static int counter = 0;
    return "user-" + std::to_string(++counter);
}

} // namespace application::use_cases
