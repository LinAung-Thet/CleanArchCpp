add_executable(CleanArchCpp
    main.cpp
)

target_link_libraries(CleanArchCpp
    PRIVATE
        interface_adapters
        infrastructure
)