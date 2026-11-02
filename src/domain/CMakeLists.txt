# Domain: ゲームのルール（ヘッダのみ・純粋関数・I/O なし）。何にも依存しない。
add_library(ngg_domain INTERFACE)
add_library(ngg::domain ALIAS ngg_domain)

target_include_directories(ngg_domain
    INTERFACE
        ${PROJECT_SOURCE_DIR}/src
)
