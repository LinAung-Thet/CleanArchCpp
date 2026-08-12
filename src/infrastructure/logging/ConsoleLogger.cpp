#include "ConsoleLogger.h"
#include <iostream>

namespace infrastructure::logging {

void ConsoleLogger::info(const std::string& msg) const {
    std::cout << "[INFO] " << msg << '\n';
}

void ConsoleLogger::error(const std::string& msg) const {
    std::cerr << "[ERROR] " << msg << '\n';
}

} // namespace infrastructure::logging
