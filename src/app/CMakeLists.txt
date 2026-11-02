# Application: ゲーム進行の制御と、外部に求める機能のインターフェース（ports）。
add_library(ngg_app
    GameController.cpp
)
add_library(ngg::app ALIAS ngg_app)

target_include_directories(ngg_app
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

# 公開ヘッダ（GameController.h）が domain を使うので PUBLIC
target_link_libraries(ngg_app
    PUBLIC
        ngg::domain
    PRIVATE
        ngg::warnings
)
