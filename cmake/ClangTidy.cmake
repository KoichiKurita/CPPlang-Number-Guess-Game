# clang-tidy をコンパイルと同時に実行する設定。
# 自分たちのターゲットにだけ ngg_enable_clang_tidy() で適用する（googletest などには適用しない）。

option(NGG_ENABLE_CLANG_TIDY "Run clang-tidy on project targets while compiling" OFF)

if(NGG_ENABLE_CLANG_TIDY)
    find_program(NGG_CLANG_TIDY_EXE NAMES clang-tidy REQUIRED)
    message(STATUS "clang-tidy: ${NGG_CLANG_TIDY_EXE}")
endif()

function(ngg_enable_clang_tidy target)
    if(NGG_ENABLE_CLANG_TIDY)
        set_target_properties(${target} PROPERTIES
            # GCC 用の警告オプションを clang-tidy（clang）が知らなくてもエラーにしない
            CXX_CLANG_TIDY "${NGG_CLANG_TIDY_EXE};--extra-arg=-Wno-unknown-warning-option"
        )
    endif()
endfunction()
