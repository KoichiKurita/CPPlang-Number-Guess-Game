# Domain (header-only, pure functions)
add_library(domain INTERFACE)

target_include_directories(domain
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Application
add_library(app
    app/GameController.cpp
)

target_link_libraries(app
    PRIVATE domain
)

target_include_directories(app
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Infrastructure
add_library(infra
    infra/RandomMT.cpp
    infra/CsvScoreRepository.cpp
)

target_include_directories(infra
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# UI
add_library(ui
    ui/ConsoleUI.cpp
)

target_include_directories(ui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Executable
add_executable(NumberGuessGame
    main.cpp
)

target_link_libraries(NumberGuessGame
    PRIVATE app infra ui
)
