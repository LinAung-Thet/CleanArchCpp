add_library(domain STATIC
    entities/User.cpp
    entities/Product.cpp
    value_objects/Email.cpp
    repositories/IUserRepository.h
)

target_include_directories(domain
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)