#pragma once
#include <string>
#include <stdexcept>

namespace domain::value_objects {

class Email {
public:
    explicit Email(std::string value);

    const std::string& value() const;

private:
    std::string value_;
    static bool isValid(const std::string& email);
};

} // namespace domain::value_objects
