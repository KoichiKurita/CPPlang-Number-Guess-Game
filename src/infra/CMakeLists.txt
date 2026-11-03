# Infrastructure: app のポートの実装（乱数・永続化）。
add_library(ngg_infra
    RandomMT.cpp
    Csv.cpp
    CsvScoreRepository.cpp
    ScoreFilePath.cpp
)
add_library(ngg::infra ALIAS ngg_infra)
ngg_enable_clang_tidy(ngg_infra)

target_include_directories(ngg_infra
    PUBLIC
        ${PROJECT_SOURCE_DIR}/src
)

# 公開ヘッダがポート（app/ports）を継承しているので PUBLIC
target_link_libraries(ngg_infra
    PUBLIC
        ngg::app
    PRIVATE
        ngg::warnings
)
