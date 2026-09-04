#include "ConsoleProductPresenter.h"

namespace interface_adapters::presenters{
    ConsoleProductPresenter::ConsoleProductPresenter(const infrastructure::logging::ConsoleLogger& logger) :
        logger_(logger) {}

    void ConsoleProductPresenter::showResult(const std::string& message){
        logger_.info(message);
    }
}