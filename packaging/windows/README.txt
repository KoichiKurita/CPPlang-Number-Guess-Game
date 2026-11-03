Number Guess Game (Windows x64)
================================

1 から N までの数を当てるゲームです。

起動方法
--------
  NumberGuessGameQt.exe  ... GUI 版（ダブルクリックで起動）
  NumberGuessGame.exe    ... コンソール版（コマンドプロンプトや PowerShell から起動）

  インストールは不要です。zip を展開したフォルダから、そのまま起動してください。
  フォルダ内の DLL やサブフォルダは実行に必要なので、削除・移動しないでください。

スコアの保存先
--------------
  %APPDATA%\NumberGuessGame\scores.csv
  （環境変数 NGG_SCORE_FILE で変更できます）

バージョンの確認
----------------
  NumberGuessGame.exe --version

配布物の検証
------------
  この zip は GitHub Actions で作成し、作成元を証明する署名（来歴証明）を付けています。
  GitHub CLI（gh）があれば、次のコマンドで改ざんされていないことを確認できます。

    gh attestation verify NumberGuessGame-v<バージョン>-windows-x64.zip --repo KoichiKurita/CPPlang-Number-Guess-Game

ライセンス
----------
  このソフトウェアは MIT License で公開しています（LICENSE.txt を参照）。

第三者ソフトウェア
------------------
  このパッケージには、Qt 6（LGPL v3）および Qt が依存するライブラリの DLL を
  変更せずに同梱しています。各ライブラリのライセンスは licenses フォルダを、
  パッケージ名・バージョン・入手元は licenses/PACKAGES.txt を参照してください。

ソースコード・詳細
------------------
  https://github.com/KoichiKurita/CPPlang-Number-Guess-Game
