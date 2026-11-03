#!/usr/bin/env python3
"""配布 zip の SBOM（ソフトウェア部品表）を SPDX 2.3 の JSON で作る。

部品の情報は、packaging/windows/package.sh が書き出す <名前>.packages.tsv
（同梱した DLL を提供している MSYS2 パッケージの一覧）から読み取る。

使い方:
    python3 packaging/sbom/generate_spdx.py \\
        --zip dist/NumberGuessGame-v2.2.0-windows-x64.zip \\
        --packages dist/NumberGuessGame-v2.2.0-windows-x64.packages.tsv \\
        --version 2.2.0 --repository KoichiKurita/CPPlang-Number-Guess-Game \\
        --commit <コミットハッシュ> --license-file LICENSE \\
        --output dist/NumberGuessGame-v2.2.0-windows-x64.spdx.json

同じ入力からは同じ出力になるように、作成日時は環境変数 SOURCE_DATE_EPOCH
（コミットの日時）を使い、文書の ID は入力から決まる UUID にする。
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import os
import re
import sys
import uuid
from datetime import datetime, timezone
from pathlib import Path

APP_NAME = "NumberGuessGame"
APP_LICENSE = "MIT"

# SPDX のライセンス式として扱える文字列か（識別子・WITH・AND・OR・括弧だけからなる）
_LICENSE_EXPRESSION = re.compile(r"^[A-Za-z0-9.+\-() ]+$")

# MSYS2 が「SPDX の一覧にない独自のライセンス」を表すのに使う "custom" を、
# SPDX の独自ライセンスへの参照（LicenseRef-）に置き換える
_CUSTOM_LICENSE = re.compile(r"(?<![A-Za-z0-9.\-])custom(?![A-Za-z0-9.\-])")
CUSTOM_LICENSE_REF = "LicenseRef-MSYS2-custom"
CUSTOM_LICENSE_INFO = {
    "licenseId": CUSTOM_LICENSE_REF,
    "name": "MSYS2 custom license",
    "extractedText": (
        "The MSYS2 package declares a license that is not on the SPDX License List "
        "(\"custom\"). The license texts are included in the licenses folder of the "
        "distribution package."
    ),
}


def sha256_of(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            digest.update(chunk)
    return digest.hexdigest()


def spdx_id(text: str) -> str:
    """SPDX の識別子に使えない文字を - に置き換える。"""
    return re.sub(r"[^A-Za-z0-9.-]", "-", text)


def to_license_expression(raw: str) -> tuple[str, str | None]:
    """pacman の Licenses 欄を SPDX のライセンス式に変換する。

    MSYS2 は多くのパッケージで "spdx:" 付きの SPDX 式を使っている。
    それ以外（独自のライセンス名など）は NOASSERTION とし、元の値をコメントに残す。
    """
    value = raw.strip()
    if value.startswith("spdx:"):
        expression = value[len("spdx:"):].strip()
        # MSYS2 自身の LicenseRef-（本文が手元にない独自ライセンス）は扱わない
        if expression and _LICENSE_EXPRESSION.match(expression) and "LicenseRef-" not in expression:
            return _CUSTOM_LICENSE.sub(CUSTOM_LICENSE_REF, expression), None
    return "NOASSERTION", f"License as declared by MSYS2: {value or '(none)'}"


def copyright_from(license_file: Path | None) -> str:
    if license_file is None or not license_file.is_file():
        return "NOASSERTION"
    for line in license_file.read_text(encoding="utf-8").splitlines():
        if line.strip().lower().startswith("copyright"):
            return line.strip()
    return "NOASSERTION"


def build_document(args: argparse.Namespace) -> dict:
    zip_path = Path(args.zip)
    repo_url = f"https://github.com/{args.repository}"
    tag = f"v{args.version}"
    doc_name = zip_path.stem

    epoch = int(os.environ.get("SOURCE_DATE_EPOCH", datetime.now(timezone.utc).timestamp()))
    created = datetime.fromtimestamp(epoch, timezone.utc).strftime("%Y-%m-%dT%H:%M:%SZ")
    namespace_id = uuid.uuid5(uuid.NAMESPACE_URL, f"{repo_url}/{tag}/{args.commit}/{zip_path.name}")

    app_id = f"SPDXRef-Package-{APP_NAME}"
    app = {
        "SPDXID": app_id,
        "name": APP_NAME,
        "versionInfo": args.version,
        "packageFileName": zip_path.name,
        "downloadLocation": f"{repo_url}/releases/download/{tag}/{zip_path.name}",
        "homepage": repo_url,
        "sourceInfo": f"Built by GitHub Actions from {repo_url} at commit {args.commit}",
        "filesAnalyzed": False,
        "checksums": [{"algorithm": "SHA256", "checksumValue": sha256_of(zip_path)}],
        "licenseConcluded": APP_LICENSE,
        "licenseDeclared": APP_LICENSE,
        "copyrightText": copyright_from(Path(args.license_file) if args.license_file else None),
        "supplier": "NOASSERTION",
        "primaryPackagePurpose": "APPLICATION",
        "externalRefs": [
            {
                "referenceCategory": "PACKAGE-MANAGER",
                "referenceType": "purl",
                "referenceLocator": f"pkg:github/{args.repository}@{tag}",
            }
        ],
    }

    packages = [app]
    relationships = [
        {
            "spdxElementId": "SPDXRef-DOCUMENT",
            "relationshipType": "DESCRIBES",
            "relatedSpdxElement": app_id,
        }
    ]

    with open(args.packages, encoding="utf-8", newline="") as f:
        rows = sorted(csv.DictReader(f, delimiter="\t"), key=lambda r: r["name"])
    for row in rows:
        license_declared, comment = to_license_expression(row["licenses"])
        package_id = f"SPDXRef-Package-msys2-{spdx_id(row['name'])}"
        package = {
            "SPDXID": package_id,
            "name": row["name"],
            "versionInfo": row["version"],
            "downloadLocation": "NOASSERTION",
            "filesAnalyzed": False,
            "licenseConcluded": "NOASSERTION",
            "licenseDeclared": license_declared,
            "copyrightText": "NOASSERTION",
            "supplier": "Organization: MSYS2 (https://www.msys2.org/)",
            "primaryPackagePurpose": "LIBRARY",
        }
        if row["url"].strip():
            package["homepage"] = row["url"].strip()
        if comment:
            package["comment"] = comment
        packages.append(package)
        relationships.append(
            {
                "spdxElementId": app_id,
                "relationshipType": "CONTAINS",
                "relatedSpdxElement": package_id,
            }
        )

    document = {
        "spdxVersion": "SPDX-2.3",
        "dataLicense": "CC0-1.0",
        "SPDXID": "SPDXRef-DOCUMENT",
        "name": doc_name,
        "documentNamespace": f"{repo_url}/releases/tag/{tag}/sbom/{namespace_id}",
        "creationInfo": {
            "created": created,
            "creators": [f"Tool: {APP_NAME}-generate_spdx.py"],
        },
        "packages": packages,
        "relationships": relationships,
    }
    # LicenseRef- を使ったときは、その中身（独自ライセンスの説明）も文書に含める
    if any(CUSTOM_LICENSE_REF in p["licenseDeclared"] for p in packages):
        document["hasExtractedLicensingInfos"] = [CUSTOM_LICENSE_INFO]
    return document


def main() -> int:
    parser = argparse.ArgumentParser(description="Generate an SPDX 2.3 SBOM for the Windows package")
    parser.add_argument("--zip", required=True, help="配布 zip のパス")
    parser.add_argument("--packages", required=True, help="package.sh が書き出した .packages.tsv のパス")
    parser.add_argument("--version", required=True, help="バージョン（例: 2.2.0）")
    parser.add_argument("--repository", required=True, help="GitHub のリポジトリ（owner/name）")
    parser.add_argument("--commit", required=True, help="ビルドしたコミットのハッシュ")
    parser.add_argument("--license-file", help="著作権表示を読み取る LICENSE ファイル")
    parser.add_argument("--output", required=True, help="出力する SPDX JSON のパス")
    args = parser.parse_args()

    document = build_document(args)
    Path(args.output).write_text(
        json.dumps(document, indent=2, ensure_ascii=False) + "\n", encoding="utf-8"
    )
    print(f"Wrote {args.output} ({len(document['packages']) - 1} third-party packages)")
    return 0


if __name__ == "__main__":
    sys.exit(main())
