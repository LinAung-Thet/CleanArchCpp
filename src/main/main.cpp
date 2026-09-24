
#include "../infrastructure/persistence/UserRepositoryInMemory.h"   
#include "../infrastructure/logging/ConsoleLogger.h"
#include "../application/use_cases/RegisterUser.h"
#include "../application/use_cases/RegisterProd.h"
#include "../interface_adapters/presenters/ConsoleUserPresenter.h"
#include "../interface_adapters/presenters/ConsoleProductPresenter.h"
#include "../interface_adapters/controllers/UserController.h"
#include "../interface_adapters/controllers/ProductController.h"
#include "../infrastructure/persistence/UserRepositoryInDb.h"
#include "../infrastructure/persistence/ProductRepositoryInDb.h"
#include "../infrastructure/persistence/DbAdapter.h"
#include "../infrastructure/persistence/sqlserver/SqlServerConnection.h"
#include "../infrastructure/persistence/sqlserver/SqlServerHelper.h"
#include "../infrastructure/persistence/PostgreDB/PostgresConnection.h"
#include "../infrastructure/persistence/PostgreDB/PostgreSqlHelper.h"

int main() {
    // SQL Server connection string
    // std::string connStr =
    //     "DRIVER={ODBC Driver 17 for SQL Server};"
    //     "SERVER=localhost,64420;"
    //     "DATABASE=Conveyor;"
    //     "UID=sa;"
    //     "PWD=lat123456;"
    //     "Encrypt=no;";

    // PostgreSQL connection string
    std::string connStr =
    "DRIVER={PostgreSQL Unicode};"
    "SERVER=localhost;"
    "PORT=3005;"
    "DATABASE=Conveyor;"
    "UID=postgres;"
    "PWD=password;";

    using User = domain::entities::User;
    using Product = domain::entities::Product;

    // infrastructure::persistence::sqlserver::SqlServer dbConnection;
    // dbConnection.connect(connStr);

    // infrastructure::persistence::sqlserver::SqlServerHelper sqlHelper(dbConnection.connection());

    using infrastructure::persistence::postgresql::PostgresConnection;

    PostgresConnection dbConnection;
    dbConnection.connect(connStr);

    infrastructure::persistence::PostgreSqlHelper sqlHelper(dbConnection.connection());

    infrastructure::persistence::DbAdapter<User> userAdapter(sqlHelper);
    infrastructure::persistence::UserRepositoryInDb userRepo(userAdapter);

    infrastructure::persistence::DbAdapter<Product> productAdapter(sqlHelper);
    infrastructure::persistence::ProductRepositoryInDb productRepo(productAdapter);

    infrastructure::logging::ConsoleLogger logger;
    
    application::use_cases::RegisterUser registerUserUseCase(userRepo);
    interface_adapters::presenters::ConsoleUserPresenter presenter(logger);
    interface_adapters::controllers::UserController controller(registerUserUseCase, presenter);

    application::use_cases::RegisterProd registerProductUseCase(productRepo);
    interface_adapters::presenters::ConsoleProductPresenter prodPresenter(logger);
    interface_adapters::controllers::ProductController prodController(registerProductUseCase, prodPresenter);


    controller.registerUser("Linl", "linl@example.com");
    controller.registerUser("Linl", "linl@example.com"); // duplicate to show behavior

    double price = 19.99;
    prodController.registerProd("Widget", price);
    prodController.registerProd("Widget", price); // duplicate to show behavior

    return 0;
}
