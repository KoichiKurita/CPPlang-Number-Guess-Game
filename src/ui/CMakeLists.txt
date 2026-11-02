# UI: app のポート（IGameUI）のコンソール実装。
add_library(ngg_ui
    ConsoleUI.cpp
)
add_library(ngg::ui ALIAS ngg_ui)

target_include_directories(ngg_ui
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

# 公開ヘッダが IGameUI を継承しているので app は PUBLIC。
# 入力検証（domain/InputParser.h）は .cpp だけで使うが、domain は app 経由で既に PUBLIC。
target_link_libraries(ngg_ui
    PUBLIC
        ngg::app
    PRIVATE
        ngg::warnings
)
