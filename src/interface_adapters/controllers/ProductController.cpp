#include "ProductController.h"

namespace interface_adapters::controllers{
    ProductController::ProductController(
        application::use_cases::RegisterProd& useCase,
        presenters::ConsoleProductPresenter& presenter) :
        useCase_(useCase), presenter_(presenter)
        {}

    void ProductController::registerProd(const std::string& name, double& price) {
        application::dto::RegisterProdRequest request{ name, price };
        auto info = useCase_.execute(request);
        presenter_.showResult(info);
    }
}