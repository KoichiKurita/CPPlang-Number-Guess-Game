# 各レイヤーをライブラリとして定義する。
# インクルードはすべて src/ を起点に書く（例: #include "app/ports/IRandom.h"）。
add_subdirectory(domain)
add_subdirectory(app)
add_subdirectory(infra)
add_subdirectory(ui)

# 実行ファイル（Composition Root: 具体クラスを組み立てる唯一の場所）
add_executable(NumberGuessGame
    main.cpp
)
ngg_enable_clang_tidy(NumberGuessGame)

target_link_libraries(NumberGuessGame
    PRIVATE
        ngg::app
        ngg::infra
        ngg::ui
        ngg::version
        ngg::warnings
)

install(TARGETS NumberGuessGame RUNTIME DESTINATION ${NGG_INSTALL_BINDIR})
