#pragma once

#include <string>

// 1ゲーム分のスコア
struct ScoreRecord {
    std::string player;      // プレイヤー名（domain/PlayerName.h で検証済みの値）
    std::string difficulty;  // 難易度名（"Easy" など）
    int attempts;            // 正解までの試行回数
};
