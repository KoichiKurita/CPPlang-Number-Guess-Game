find_package(Qt6 6.2 REQUIRED COMPONENTS Test)

add_executable(GameWindowTest
    GameWindowTest.cpp
)

target_include_directories(GameWindowTest
    PRIVATE
        ${PROJECT_SOURCE_DIR}/tests   # MockRandom.h / FakeScoreRepository.h を再利用する
)

target_link_libraries(GameWindowTest
    PRIVATE
        Qt6::Test
        ngg::gui
        ngg::warnings
)

add_test(NAME GameWindowTest COMMAND GameWindowTest)

# 画面のない環境（CI）でも動くように、描画先を offscreen にする
set_tests_properties(GameWindowTest PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
