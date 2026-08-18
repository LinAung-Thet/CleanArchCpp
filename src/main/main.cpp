#include "../infrastructure/persistence/InMemoryUserRepository.h"   
#include "../infrastructure/logging/ConsoleLogger.h"
#include "../application/use_cases/RegisterUser.h"
#include "../interface_adapters/presenters/ConsoleUserPresenter.h"
#include "../interface_adapters/controllers/UserController.h"
#include "../infrastructure/persistence/InDbUserRepository.h"
#include "../infrastructure/persistence/sqlserver/SqlServerConnection.h"

int main() {
    std::string connStr =
        "DRIVER={ODBC Driver 17 for SQL Server};"
        "SERVER=localhost,64420;"
        "DATABASE=Conveyor;"
        "UID=sa;"
        "PWD=lat123456;"
        "Encrypt=no;";
        // "Encrypt=yes;TrustServerCertificate=yes;";

    infrastructure::persistence::sqlserver::SqlServerConnection dbConnection(connStr);
    infrastructure::persistence::InDbUserRepository userRepo(dbConnection, connStr);
    infrastructure::logging::ConsoleLogger logger;

    application::use_cases::RegisterUser registerUserUseCase(userRepo);
    interface_adapters::presenters::ConsoleUserPresenter presenter(logger);
    interface_adapters::controllers::UserController controller(registerUserUseCase, presenter);

    controller.registerUser("Linb", "linb@example.com");
    controller.registerUser("Linb", "linb@example.com"); // duplicate to show behavior

    return 0;
}
