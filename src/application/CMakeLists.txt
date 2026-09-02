add_library(application STATIC
    use_cases/RegisterUser.cpp
    use_cases/RegisterProd.cpp
    dto/RegisterUserRequest.h
    dto/RegisterProdRequest.h
)

target_include_directories(application
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(application
    PUBLIC
        domain
)