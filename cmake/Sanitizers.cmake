# AddressSanitizer / UndefinedBehaviorSanitizer の設定。
# メモリ破壊・範囲外アクセス・未定義動作を実行時に検出する（GCC / Clang のみ）。
#
# add_compile_options はこのファイルを include した後に追加されるディレクトリ全体
# （src と tests、googletest を含む）に適用される。

option(NGG_ENABLE_SANITIZERS "Enable AddressSanitizer and UndefinedBehaviorSanitizer" OFF)

if(NGG_ENABLE_SANITIZERS)
    if(MSVC OR MINGW)
        message(FATAL_ERROR
            "NGG_ENABLE_SANITIZERS is supported only with GCC/Clang on Linux or macOS")
    endif()

    add_compile_options(
        -fsanitize=address,undefined
        -fno-omit-frame-pointer       # スタックトレースを正確にする
        -fno-sanitize-recover=all     # 未定義動作を検出したら即座に異常終了（テストを失敗させる）
    )
    add_link_options(
        -fsanitize=address,undefined
    )
    message(STATUS "Sanitizers enabled: address, undefined")
endif()
