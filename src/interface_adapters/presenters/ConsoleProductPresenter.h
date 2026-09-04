#pragma once

#include "../../infrastructure/logging/ConsoleLogger.h"

namespace interface_adapters::presenters{
    class ConsoleProductPresenter{
        public:
            explicit ConsoleProductPresenter(const infrastructure::logging::ConsoleLogger&);

            void showResult(const std::string&);
        private:
            infrastructure::logging::ConsoleLogger logger_;
    };
}