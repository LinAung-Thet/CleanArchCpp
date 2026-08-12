#include "UserController.h"

namespace interface_adapters::controllers {

UserController::UserController(application::use_cases::RegisterUser& useCase,
                               presenters::ConsoleUserPresenter& presenter)
    : useCase_(useCase),
      presenter_(presenter) {}

void UserController::registerUser(const std::string& name, const std::string& email) {
    application::dto::RegisterUserRequest req{ name, email };
    auto result = useCase_.execute(req);
    presenter_.showResult(result);
}

} // namespace interface_adapters::controllers
