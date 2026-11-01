cmake_minimum_required(VERSION 3.16)

project(NumberGuessGame
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

# テスト（-DBUILD_TESTING=OFF で無効化できる）
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
