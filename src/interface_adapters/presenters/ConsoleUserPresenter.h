#pragma once
#include <string>

#include "../../infrastructure/logging/ConsoleLogger.h"

namespace interface_adapters::presenters {

class ConsoleUserPresenter {
public:
    explicit ConsoleUserPresenter(infrastructure::logging::ConsoleLogger& logger);

    void showResult(const std::string& message);

private:
    infrastructure::logging::ConsoleLogger& logger_;
};

} // namespace interface_adapters::presenters
