# プロジェクト共通の警告設定。
# 自分たちのターゲットにだけ PRIVATE でリンクして使う（googletest には適用しない）。

option(NGG_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

add_library(ngg_warnings INTERFACE)
add_library(ngg::warnings ALIAS ngg_warnings)

if(MSVC)
    target_compile_options(ngg_warnings INTERFACE
        /W4             # 高い警告レベル
        /permissive-    # 標準規格に厳密に従う
    )
    if(NGG_WARNINGS_AS_ERRORS)
        target_compile_options(ngg_warnings INTERFACE /WX)
    endif()
else()
    target_compile_options(ngg_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic
        -Wshadow            # 変数の隠蔽
        -Wconversion        # 暗黙の型変換による値の欠落
        -Wsign-conversion   # 符号付き/符号なしの暗黙変換
        -Wnon-virtual-dtor  # 仮想関数を持つのに仮想デストラクタがない
        -Wold-style-cast    # C スタイルキャスト
        -Woverloaded-virtual
    )
    if(NGG_WARNINGS_AS_ERRORS)
        target_compile_options(ngg_warnings INTERFACE -Werror)
    endif()
endif()
