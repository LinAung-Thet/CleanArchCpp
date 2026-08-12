#include "ConsoleUserPresenter.h"

namespace interface_adapters::presenters {

ConsoleUserPresenter::ConsoleUserPresenter(infrastructure::logging::ConsoleLogger& logger)
    : logger_(logger) {}

void ConsoleUserPresenter::showResult(const std::string& message) {
    logger_.info(message);
}

} // namespace interface_adapters::presenters
