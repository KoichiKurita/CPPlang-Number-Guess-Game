# Qt 版 GUI（-DNGG_BUILD_QT=ON のときだけビルドする）
find_package(Qt6 6.2 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)   # Q_OBJECT を含むクラスのために moc を自動実行する

# 画面（テストからも使うのでライブラリにする）
add_library(ngg_gui
    GameWindow.cpp
)
add_library(ngg::gui ALIAS ngg_gui)
ngg_enable_clang_tidy(ngg_gui)

target_include_directories(ngg_gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(ngg_gui
    PUBLIC
        Qt6::Widgets
        ngg::app
    PRIVATE
        ngg::warnings
)

# 実行ファイル（Windows ではコンソールウィンドウを出さない）
add_executable(NumberGuessGameQt WIN32
    main.cpp
)
ngg_enable_clang_tidy(NumberGuessGameQt)

target_link_libraries(NumberGuessGameQt
    PRIVATE
        ngg::gui
        ngg::infra
        ngg::version
        ngg::warnings
)

install(TARGETS NumberGuessGameQt RUNTIME DESTINATION ${NGG_INSTALL_BINDIR})

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
