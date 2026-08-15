add_library(infrastructure STATIC
    persistence/InMemoryUserRepository.cpp
    persistence/InDbUserRepository.cpp
    persistence/sqlserver/SqlServerRepository.cpp
    logging/ConsoleLogger.cpp
)

# Link ODBC
find_package(ODBC REQUIRED)

target_link_libraries(infrastructure PUBLIC domain ODBC::ODBC)
target_include_directories(infrastructure PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
