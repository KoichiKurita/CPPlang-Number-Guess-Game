include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

# MSVC でランタイム設定の不一致によるリンクエラーを防ぐ（MinGW では影響なし）
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# gmock / gtest 自体のインストール対象化は不要
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

add_executable(GameTests
    GameLogicTest.cpp
    InputParserTest.cpp
    GameControllerTest.cpp
    ConsoleUITest.cpp
)

target_link_libraries(GameTests
    PRIVATE
        GTest::gtest_main
        app
        infra
        ui
)

# テストケースを1件ずつ CTest に登録する（失敗したテスト名が CI ログに出る）
include(GoogleTest)
gtest_discover_tests(GameTests)
