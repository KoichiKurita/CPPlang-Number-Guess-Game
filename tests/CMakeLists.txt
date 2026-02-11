include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/release-1.14.0.zip
)

FetchContent_MakeAvailable(googletest)

add_executable(GameTests
    GameControllerTest.cpp
)

target_link_libraries(GameTests
    PRIVATE
        gtest_main
        app
        infra
        ui
)

enable_testing()
add_test(NAME GameTests COMMAND GameTests)
