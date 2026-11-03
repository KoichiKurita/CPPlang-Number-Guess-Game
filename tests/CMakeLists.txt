include(FetchContent)

# googletest はタグではなくコミットハッシュで固定する（タグは後から付け替えられるため）。
# 新しいリリースは .github/workflows/check-googletest.yml が毎週確認し、Issue で知らせる。
# 更新するときは、次の 2 行をそろえて書き換える（バージョンとコミットの一致も同じワークフローで検証する）。
set(NGG_GOOGLETEST_VERSION v1.18.0)
set(NGG_GOOGLETEST_COMMIT 063de7e9578f82b369302001269680b4b1553359)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/${NGG_GOOGLETEST_COMMIT}.zip
    # 解凍したファイルの更新日時を「解凍した時刻」にする。
    # zip の中の古い日時のままだと、版を上げてもヘッダーが新しくなったと判断されず、
    # テストが古いヘッダーのまま再コンパイルされない
    DOWNLOAD_EXTRACT_TIMESTAMP FALSE
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
    GameSessionTest.cpp
    ConsoleUITest.cpp
    RandomMTTest.cpp
    PlayerNameTest.cpp
    HighScoreTest.cpp
    CsvTest.cpp
    CsvScoreRepositoryTest.cpp
    ScoreFilePathTest.cpp
    SystemClockTest.cpp
    TimestampTest.cpp
)
ngg_enable_clang_tidy(GameTests)

# MSVC では std::getenv が非推奨の警告（C4996）になるため、テストでは抑止する
if(MSVC)
    target_compile_definitions(GameTests PRIVATE _CRT_SECURE_NO_WARNINGS)
endif()

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
