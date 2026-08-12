add_library(infrastructure STATIC
    persistence/InMemoryUserRepository.cpp
    logging/ConsoleLogger.cpp
)

target_include_directories(infrastructure
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(infrastructure
    PUBLIC
        domain
)