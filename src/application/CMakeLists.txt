add_library(application STATIC
    use_cases/RegisterUser.cpp
    dto/RegisterUserRequest.h
)

target_include_directories(application
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(application
    PUBLIC
        domain
)