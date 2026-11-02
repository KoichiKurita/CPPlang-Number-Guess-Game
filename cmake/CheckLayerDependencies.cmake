# レイヤー間の依存ルールを検査するスクリプト（アーキテクチャテスト）。
# 使い方: cmake -DSRC_DIR=<src ディレクトリ> -P CheckLayerDependencies.cmake
#
# 依存の向き:  ui ──▶ app ◀── infra
#                 └──▶ domain ◀──┘
#   domain : 何にも依存しない
#   app    : domain のみ（ポート = インターフェースを定義する）
#   infra  : app のポートを実装する（ui には依存しない）
#   ui     : app のポートを実装する（infra には依存しない）

if(NOT DEFINED SRC_DIR)
    message(FATAL_ERROR "SRC_DIR is not set")
endif()

set(FORBIDDEN_domain app infra ui)
set(FORBIDDEN_app    infra ui)
set(FORBIDDEN_infra  ui)
set(FORBIDDEN_ui     infra)

set(violations 0)

foreach(layer domain app infra ui)
    file(GLOB_RECURSE files "${SRC_DIR}/${layer}/*.h" "${SRC_DIR}/${layer}/*.cpp")

    foreach(file ${files})
        file(RELATIVE_PATH rel "${SRC_DIR}" "${file}")
        file(STRINGS "${file}" lines REGEX "^[ \t]*#[ \t]*include[ \t]*\"")

        foreach(line ${lines})
            string(REGEX MATCH "\"([^\"]+)\"" _ "${line}")
            set(inc "${CMAKE_MATCH_1}")

            if(inc MATCHES "\\.\\./")
                message(SEND_ERROR "${rel}: relative include is not allowed: \"${inc}\"")
                math(EXPR violations "${violations} + 1")
            endif()

            foreach(bad ${FORBIDDEN_${layer}})
                if(inc MATCHES "^${bad}/")
                    message(SEND_ERROR "${rel}: layer '${layer}' must not depend on '${bad}': \"${inc}\"")
                    math(EXPR violations "${violations} + 1")
                endif()
            endforeach()
        endforeach()
    endforeach()
endforeach()

if(violations GREATER 0)
    message(FATAL_ERROR "${violations} layer dependency violation(s) found")
endif()
message(STATUS "Layer dependencies OK")
