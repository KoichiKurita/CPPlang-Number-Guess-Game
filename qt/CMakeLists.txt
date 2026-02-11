find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(QtNumberGuess
    main.cpp
)

target_link_libraries(QtNumberGuess
    PRIVATE
        Qt6::Widgets
        app
        infra
)
