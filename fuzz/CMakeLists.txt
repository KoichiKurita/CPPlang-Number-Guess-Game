# ファジング（libFuzzer）。Clang でのみビルドできる（-DNGG_BUILD_FUZZERS=ON、プリセット fuzz）。
#
# libFuzzer は、入力を少しずつ変えながら関数を大量に呼び出し、新しいコードの経路を通る入力を
# 集めていく。AddressSanitizer / UndefinedBehaviorSanitizer と組み合わせ、
# クラッシュ・メモリ破壊・未定義動作、および各関数の中で確認している性質の違反を検出する。

if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "NGG_BUILD_FUZZERS requires Clang (libFuzzer)")
endif()

set(NGG_FUZZ_SANITIZERS -fsanitize=fuzzer,address,undefined)

# ファジング用の実行ファイルを作り、シードの入力（fuzz/corpus/<コーパス名>）で
# 一通り実行するテストを CTest に登録する（毎回の CI で、過去に見つけた入力の回帰を確認する）
function(ngg_add_fuzzer name source corpus)
    add_executable(${name} ${source})
    target_compile_options(${name} PRIVATE
        ${NGG_FUZZ_SANITIZERS}
        -fno-omit-frame-pointer     # スタックトレースを正確にする
        -fno-sanitize-recover=all   # 未定義動作を検出したら、その入力で即座に止める
    )
    target_link_options(${name} PRIVATE ${NGG_FUZZ_SANITIZERS})
    target_link_libraries(${name} PRIVATE ngg::infra ngg::warnings)

    add_test(
        NAME ${name}.corpus
        COMMAND ${name} -runs=0 ${CMAKE_CURRENT_SOURCE_DIR}/corpus/${corpus}
    )
endfunction()

ngg_add_fuzzer(fuzz_input_parser FuzzInputParser.cpp input_parser)
ngg_add_fuzzer(fuzz_player_name  FuzzPlayerName.cpp  player_name)
ngg_add_fuzzer(fuzz_csv          FuzzCsv.cpp         csv)
