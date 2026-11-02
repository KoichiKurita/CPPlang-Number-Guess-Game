#pragma once

#include <filesystem>

// スコアファイルの既定の保存先を決める。
//  1. 環境変数 NGG_SCORE_FILE があればそのパス
//  2. Windows: %APPDATA%\NumberGuessGame\scores.csv
//     それ以外: $XDG_DATA_HOME/NumberGuessGame/scores.csv
//               （未設定なら ~/.local/share/NumberGuessGame/scores.csv）
//  3. どれも取得できなければ カレントディレクトリの scores.csv
std::filesystem::path defaultScoreFilePath();
