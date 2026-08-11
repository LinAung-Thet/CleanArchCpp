#include "Email.h"

namespace domain::value_objects {

Email::Email(std::string value)
    : value_(std::move(value)) 
{
    if (!isValid(value_)) {
        throw std::invalid_argument("Invalid email");
    }
}

const std::string& Email::value() const {
    return value_;
}

bool Email::isValid(const std::string& email) {
    return !email.empty() && email.find('@') != std::string::npos;
}

} // namespace domain::value_objects
