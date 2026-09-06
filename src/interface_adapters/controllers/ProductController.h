#pragma once

#include "../../application/use_cases/RegisterProd.h"
#include "../presenters/ConsoleProductPresenter.h"

namespace interface_adapters::controllers{
    class ProductController{
        public:
            ProductController(
                application::use_cases::RegisterProd&,
                presenters::ConsoleProductPresenter&);
            void registerProd(const std::string&, double&);

        private:
            application::use_cases::RegisterProd& useCase_;
            presenters::ConsoleProductPresenter& presenter_;
    };
}