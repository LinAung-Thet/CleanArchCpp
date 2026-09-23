add_library(infrastructure STATIC
    persistence/UserRepositoryInMemory.cpp
    persistence/UserRepositoryInDb.cpp
    persistence/ProductRepositoryInDb.cpp
    persistence/DbAdapter.h
    persistence/IDatabaseConnection.h
    persistence/ISqlHelper.h
    persistence/SqlTraits.h
    persistence/sqlserver/SqlServerHelper.h
    persistence/sqlserver/SqlServerConnection.h
    logging/ConsoleLogger.cpp
)

# Link PostgreSQL
include_directories("C:/Program Files/PostgreSQL/18/lib")

# Link ODBC
find_package(ODBC REQUIRED)

target_link_libraries(infrastructure PUBLIC domain ODBC::ODBC)
target_link_libraries(infrastructure libpq)
target_include_directories(infrastructure PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
