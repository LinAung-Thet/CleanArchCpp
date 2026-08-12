#pragma once
#include <string>

#include "../../application/use_cases/RegisterUser.h"
#include "../../application/dto/RegisterUserRequest.h"
#include "../presenters/ConsoleUserPresenter.h"

namespace interface_adapters::controllers {

class UserController {
public:
    UserController(application::use_cases::RegisterUser& useCase,
                   presenters::ConsoleUserPresenter& presenter);

    void registerUser(const std::string& name, const std::string& email);

private:
    application::use_cases::RegisterUser& useCase_;
    presenters::ConsoleUserPresenter& presenter_;
};

} // namespace interface_adapters::controllers
