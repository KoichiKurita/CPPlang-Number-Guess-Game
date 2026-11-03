#pragma once

#include <cstdint>
#include <optional>
#include <string>

// 1ゲーム分のスコア
struct ScoreRecord {
    std::string player;     // プレイヤー名（domain/PlayerName.h で検証済みの値）
    std::string difficulty; // 難易度名（"Easy" など）
    int attempts;           // 正解までの試行回数
    // 遊んだ日時（UNIX 時間・UTC）。日時を記録していなかった古い形式の記録では空
    std::optional<std::int64_t> playedAt = std::nullopt;
};
