cmake_minimum_required(VERSION 3.16)

project(NumberGuessGame
    VERSION 1.1.0
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


# Qt 版 GUI（Qt6 が必要なので既定では無効）
option(NGG_BUILD_QT "Build the Qt GUI (requires Qt6)" OFF)
if(NGG_BUILD_QT)
    add_subdirectory(qt)
endif()
