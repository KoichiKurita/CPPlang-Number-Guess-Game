#!/usr/bin/env bash
# Windows 用の配布 zip を作る（MSYS2 MINGW64 シェルで実行する）
#
#   使い方: packaging/windows/package.sh <ビルドフォルダ> <出力フォルダ> <バージョン>
#   例:     packaging/windows/package.sh build/release dist 2.0.0
#
# 手順:
#   1. cmake --install で実行ファイルを集める
#   2. windeployqt で Qt の DLL とプラグインを集める
#   3. 依存している DLL を objdump で調べ、MSYS2 から再帰的に同梱する
#      （MSYS2 の windeployqt は Qt 以外の DLL を集めないため）
#   4. ライセンス文書を同梱し、zip と SHA-256 チェックサムを作る
set -euo pipefail

if [[ $# -ne 3 ]]; then
    echo "usage: $0 <build-dir> <output-dir> <version>" >&2
    exit 2
fi

build_dir=$1
out_dir=$2
version=$3
name="NumberGuessGame-v${version}-windows-x64"
stage="${out_dir}/${name}"
script_dir=$(cd "$(dirname "$0")" && pwd)
repo_root=$(cd "${script_dir}/../.." && pwd)

: "${MINGW_PREFIX:?run this script in the MSYS2 MINGW64 shell}"
mingw_bin="${MINGW_PREFIX}/bin"
system_dir="$(cygpath -u "${SYSTEMROOT:-C:\\Windows}")/System32"

# windeployqt のコマンド名は配布元によって異なる
windeployqt=$(command -v windeployqt6 || command -v windeployqt-qt6 || command -v windeployqt || true)
if [[ -z ${windeployqt} ]]; then
    echo "error: windeployqt not found (install mingw-w64-x86_64-qt6-base)" >&2
    exit 1
fi

# ---- 1. 実行ファイル -------------------------------------------------------
echo "==> Staging ${stage}"
rm -rf "${stage}" "${out_dir}/${name}.zip" "${out_dir}/${name}.zip.sha256"
mkdir -p "${stage}"
cmake --install "${build_dir}" --prefix "${stage}"

# ---- 2. Qt の DLL とプラグイン ---------------------------------------------
echo "==> Deploying Qt with ${windeployqt##*/}"
"${windeployqt}" --no-translations --no-system-d3d-compiler --no-opengl-sw \
    "${stage}/NumberGuessGameQt.exe"

if [[ ! -e "${stage}/platforms/qwindows.dll" ]]; then
    echo "error: Qt platform plugin (platforms/qwindows.dll) was not deployed" >&2
    exit 1
fi

# ---- 3. 依存 DLL を再帰的に同梱する ----------------------------------------
# PE ファイル（exe / dll）が直接インポートしている DLL 名を列挙する
imports_of() {
    objdump -p "$1" | sed -n 's/^[[:space:]]*DLL Name: //p' | tr -d '\r'
}

# Windows に標準で入っている DLL か（同梱しない）
is_system_dll() {
    local lower=${1,,}
    [[ ${lower} == api-ms-* || ${lower} == ext-ms-* ]] && return 0
    [[ -e "${system_dir}/$1" ]]
}

echo "==> Bundling dependent DLLs from ${mingw_bin}"
bundled=()
missing=()
mapfile -t queue < <(find "${stage}" -type f \( -iname '*.exe' -o -iname '*.dll' \))

while ((${#queue[@]} > 0)); do
    file=${queue[0]}
    queue=("${queue[@]:1}")

    while IFS= read -r dll; do
        [[ -e "${stage}/${dll}" ]] && continue   # 同梱済み（Windows はファイル名の大文字小文字を区別しない）
        is_system_dll "${dll}" && continue

        if [[ -e "${mingw_bin}/${dll}" ]]; then
            cp "${mingw_bin}/${dll}" "${stage}/"
            bundled+=("${dll}")
            queue+=("${stage}/${dll}")          # 同梱した DLL の依存も調べる
            echo "    + ${dll}"
        else
            missing+=("${dll} (required by ${file#"${stage}/"})")
        fi
    done < <(imports_of "${file}")
done

if ((${#missing[@]} > 0)); then
    printf 'error: DLL not found: %s\n' "${missing[@]}" >&2
    exit 1
fi
echo "    ${#bundled[@]} additional DLL(s) bundled"

# ---- 4. ライセンス・説明書・zip ----------------------------------------------
# 同梱した DLL を提供している MSYS2 パッケージのライセンス文書をすべて含める
echo "==> Collecting third-party licenses"
mkdir -p "${stage}/licenses"
mapfile -t packages < <(
    find "${stage}" -maxdepth 1 -type f -iname '*.dll' -printf '%f\n' |
    while IFS= read -r dll; do
        if [[ -e "${mingw_bin}/${dll}" ]]; then
            pacman -Qoq "${mingw_bin}/${dll}" 2>/dev/null || true
        fi
    done | sort -u
)
for pkg in "${packages[@]}"; do
    license_dir="${MINGW_PREFIX}/share/licenses/${pkg#"${MINGW_PACKAGE_PREFIX}-"}"
    if [[ -d ${license_dir} ]]; then
        cp -r "${license_dir}" "${stage}/licenses/"
    fi
    pacman -Qi "${pkg}" | sed -n 's/^\(Name\|Version\|Licenses\|URL\) *: /\1: /p' >> "${stage}/licenses/PACKAGES.txt"
    echo >> "${stage}/licenses/PACKAGES.txt"
done

cp "${script_dir}/README.txt" "${stage}/README.txt"
if [[ -f "${repo_root}/LICENSE" ]]; then
    cp "${repo_root}/LICENSE" "${stage}/LICENSE.txt"
fi

echo "==> Creating ${name}.zip"
(
    cd "${out_dir}"
    cmake -E tar cf "${name}.zip" --format=zip "${name}"
    sha256sum "${name}.zip" > "${name}.zip.sha256"
)
echo "==> Done: ${out_dir}/${name}.zip"
cat "${out_dir}/${name}.zip.sha256"
