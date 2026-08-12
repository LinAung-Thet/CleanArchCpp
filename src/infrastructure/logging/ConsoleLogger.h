#pragma once
#include <string>

namespace infrastructure::logging {

class ConsoleLogger {
public:
    void info(const std::string& msg) const;
    void error(const std::string& msg) const;
};

} // namespace infrastructure::logging
