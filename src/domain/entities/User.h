#pragma once
#include <string>
#include "../value_objects/Email.h"

namespace domain::entities {

class User {
public:
    User(std::string id, std::string name, value_objects::Email email);

    const std::string& id() const;
    const std::string& name() const;
    const value_objects::Email& email() const;

private:
    std::string id_;
    std::string name_;
    value_objects::Email email_;
};

} // namespace domain::entities
