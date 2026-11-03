# カバレッジ計測（gcov）の設定。GCC / Clang のみ。
# 計測用のビルドは最適化を無効にして、ソースの行と実行結果を正確に対応させる。
#
# add_compile_options はこのファイルを include した後に追加されるディレクトリ全体に適用される。
# googletest なども計測されるが、レポートを作るときに自分たちのコードだけに絞り込む。

option(NGG_ENABLE_COVERAGE "Instrument the build for code coverage (gcov)" OFF)

if(NGG_ENABLE_COVERAGE)
    if(MSVC)
        message(FATAL_ERROR "NGG_ENABLE_COVERAGE is supported only with GCC/Clang")
    endif()

    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
    message(STATUS "Coverage instrumentation enabled")
endif()
