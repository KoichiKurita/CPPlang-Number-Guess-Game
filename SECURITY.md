# セキュリティポリシー / Security Policy

[English](#english) is below.

## サポートしているバージョン

最新のリリースだけを修正の対象にします。古いバージョンで問題が見つかった場合は、最新版で再現するかを確認してください。

| バージョン | サポート |
|---|---|
| 最新のリリース | ✅ |
| それより前 | ❌ |

## 脆弱性の報告

脆弱性を見つけた場合は、**公開の Issue には書かず**、GitHub の非公開の報告機能を使ってください。

- 報告用のページ: https://github.com/KoichiKurita/CPPlang-Number-Guess-Game/security/advisories/new

（リポジトリの **Security** タブ → **Report a vulnerability** からも開けます）

再現手順・影響・環境を記入して送信してください。報告の内容は、リポジトリの管理者だけが確認できます。

## 対応の流れ

1. 30 日以内に、受け取ったことを返信します
2. 内容を確認し、修正の方針と見込みをお知らせします
3. 修正版をリリースしたあと、GitHub Security Advisory で公開します。ご希望があれば、報告者としてお名前を記載します

## 対象

- ゲーム本体（コンソール版・Qt 版）のソースコード
- 配布物（Release に添付した zip・チェックサム・SBOM・証明のファイル）と、それを作る GitHub Actions のワークフロー

同梱している外部ライブラリ（Qt・ICU など）自体の脆弱性は、それぞれの開発元に報告してください。配布物が影響を受けるかは、SBOM で確認します。

---

## English

### Supported versions

Only the latest release receives security fixes.

### Reporting a vulnerability

Please **do not open a public issue**. Report it privately through GitHub's private vulnerability reporting:

- https://github.com/KoichiKurita/CPPlang-Number-Guess-Game/security/advisories/new

Please include steps to reproduce, the impact, and your environment. Reports may be written in English or Japanese.

### Disclosure process

1. You will receive an acknowledgement within 30 days.
2. We will investigate and share the planned fix and timeline.
3. After a fixed release is published, we will disclose the issue in a GitHub Security Advisory and credit you if you wish.

### Scope

The game source code (console and Qt versions), the release assets (zip, checksum, SBOM and attestation bundle), and the GitHub Actions workflows that build them. Vulnerabilities in bundled third-party libraries (such as Qt or ICU) should be reported to their upstream projects.
