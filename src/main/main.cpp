#include "../infrastructure/persistence/InMemoryUserRepository.h"   
#include "../infrastructure/logging/ConsoleLogger.h"
#include "../application/use_cases/RegisterUser.h"
#include "../interface_adapters/presenters/ConsoleUserPresenter.h"
#include "../interface_adapters/controllers/UserController.h"

int main() {
    infrastructure::persistence::InMemoryUserRepository userRepo;
    infrastructure::logging::ConsoleLogger logger;

    application::use_cases::RegisterUser registerUserUseCase(userRepo);
    interface_adapters::presenters::ConsoleUserPresenter presenter(logger);
    interface_adapters::controllers::UserController controller(registerUserUseCase, presenter);

    controller.registerUser("Lin", "lin@example.com");
    controller.registerUser("Lin", "lin@example.com"); // duplicate to show behavior

    return 0;
}
