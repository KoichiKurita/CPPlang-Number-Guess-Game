cmake_minimum_required(VERSION 3.16)

project(NumberGuessGame
    VERSION 1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)            # -std=gnu++17 ではなく標準の -std=c++17 を使う
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)    # compile_commands.json を出力（エディタ補完・clang-tidy 用）

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CompilerWarnings)
include(Sanitizers)

add_subdirectory(src)

# テスト（-DBUILD_TESTING=OFF で無効化できる）
include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
