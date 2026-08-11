#include "User.h"

namespace domain::entities {

User::User(std::string id, std::string name, value_objects::Email email)
    : id_(std::move(id)),
      name_(std::move(name)),
      email_(std::move(email)) {}

const std::string& User::id() const {
    return id_;
}

const std::string& User::name() const {
    return name_;
}

const value_objects::Email& User::email() const {
    return email_;
}

} // namespace domain::entities
