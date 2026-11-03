#!/usr/bin/env bash
# テスト実行後のカバレッジ（gcov のデータ）から、lcov でレポートを作る
#
#   使い方: scripts/coverage.sh <ビルドフォルダ> <出力フォルダ>
#   例:     scripts/coverage.sh build/coverage coverage
#
# 事前に、カバレッジ計測付きのビルドでテストを実行しておく:
#   cmake --preset coverage && cmake --build --preset coverage && ctest --preset coverage
#
# 出力:
#   <出力フォルダ>/coverage.info   ... lcov 形式のデータ（Codecov にアップロードする）
#   <出力フォルダ>/html/index.html ... ブラウザで見られるレポート
#   <出力フォルダ>/summary.txt     ... 行・関数のカバレッジ率の要約
set -euo pipefail

if [[ $# -ne 2 ]]; then
    echo "usage: $0 <build-dir> <output-dir>" >&2
    exit 2
fi

build_dir=$1
out_dir=$2
raw="${out_dir}/coverage-raw.info"
info="${out_dir}/coverage.info"

# lcov 2.x は細かな不整合をエラーにするため、レポートに影響しないものだけ無視する。
# lcov 1.x（MSYS2 など）はこのオプションを知らないので付けない
lcov_major=$(lcov --version | sed -n 's/.*version \([0-9]*\).*/\1/p')
capture_opts=()
ignore_opts=()
if ((lcov_major >= 2)); then
    capture_opts=(--ignore-errors "mismatch,inconsistent,unused,gcov")
    ignore_opts=(--ignore-errors "inconsistent,unused")
fi

mkdir -p "${out_dir}"

echo "==> Capturing coverage data from ${build_dir}"
# テストで一度も実行されなかったファイル（main.cpp など）も 0% として数えるため、
# 実行前の状態（--initial）と実行結果を合算する
lcov --capture --initial --directory "${build_dir}" --output-file "${out_dir}/base.info" \
    --quiet "${capture_opts[@]}"
lcov --capture --directory "${build_dir}" --output-file "${out_dir}/test.info" \
    --quiet "${capture_opts[@]}"
lcov --add-tracefile "${out_dir}/base.info" --add-tracefile "${out_dir}/test.info" \
    --output-file "${raw}" --quiet "${ignore_opts[@]}"
rm -f "${out_dir}/base.info" "${out_dir}/test.info"

# 自分たちのコード（src / qt）だけを残す。
# テストコード・外部ライブラリ（googletest）・Qt の自動生成コードは除く
echo "==> Filtering to project sources"
lcov --extract "${raw}" '*/src/*' '*/qt/*' \
    --output-file "${info}" --quiet "${ignore_opts[@]}"
lcov --remove "${info}" '*/_deps/*' '*/googletest/*' '*/googlemock/*' '*/qt/tests/*' '*_autogen*' \
    --output-file "${info}" --quiet "${ignore_opts[@]}"
rm -f "${raw}"

echo "==> Summary"
lcov --summary "${info}" "${ignore_opts[@]}" 2>&1 | tee "${out_dir}/summary.txt"

echo "==> Generating HTML report"
genhtml "${info}" --output-directory "${out_dir}/html" \
    --title "Number Guess Game" --quiet "${ignore_opts[@]}"
echo "==> Done: ${out_dir}/html/index.html"
