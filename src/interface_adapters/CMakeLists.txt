message(STATUS "Configuring interface_adapters layer")

add_library(interface_adapters STATIC
    controllers/UserController.cpp
    presenters/ConsoleUserPresenter.cpp
)

target_include_directories(interface_adapters
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(interface_adapters
    PUBLIC
        application
)