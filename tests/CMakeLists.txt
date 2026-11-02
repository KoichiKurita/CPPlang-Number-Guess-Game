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
    RandomMTTest.cpp
)

target_link_libraries(GameTests
    PRIVATE
        GTest::gtest_main
        ngg::app
        ngg::infra
        ngg::ui
        ngg::warnings
)

# テストケースを1件ずつ CTest に登録する（失敗したテスト名が CI ログに出る）
include(GoogleTest)
gtest_discover_tests(GameTests)

# アーキテクチャテスト: レイヤー間の依存ルールと相対インクルードの禁止を検査する
add_test(
    NAME LayerDependencies
    COMMAND ${CMAKE_COMMAND}
        -DSRC_DIR=${PROJECT_SOURCE_DIR}/src
        -P ${PROJECT_SOURCE_DIR}/cmake/CheckLayerDependencies.cmake
)
