#include "../infrastructure/persistence/InMemoryUserRepository.h"   
#include "../infrastructure/logging/ConsoleLogger.h"
#include "../application/use_cases/RegisterUser.h"
#include "../interface_adapters/presenters/ConsoleUserPresenter.h"
#include "../interface_adapters/controllers/UserController.h"
#include "../infrastructure/persistence/InDbUserRepository.h"

int main() {
    std::string connStr =
        "DRIVER={ODBC Driver 17 for SQL Server};"
        "SERVER=localhost,64420;"
        "DATABASE=Conveyor;"
        "UID=sa;"
        "PWD=lat123456;"
        "Encrypt=no;";
        // "Encrypt=yes;TrustServerCertificate=yes;";

    infrastructure::persistence::InDbUserRepository userRepo(connStr);
    infrastructure::logging::ConsoleLogger logger;

    application::use_cases::RegisterUser registerUserUseCase(userRepo);
    interface_adapters::presenters::ConsoleUserPresenter presenter(logger);
    interface_adapters::controllers::UserController controller(registerUserUseCase, presenter);

    controller.registerUser("Lin", "lin@example.com");
    controller.registerUser("Lin", "lin@example.com"); // duplicate to show behavior

    return 0;
}
