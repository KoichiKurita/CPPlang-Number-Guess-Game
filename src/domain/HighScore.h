#pragma once

#include "domain/ScoreRecord.h"

#include <optional>
#include <string_view>
#include <vector>

// 指定した難易度の最高記録（試行回数が最少）を返す。同点なら先に記録されたもの。
inline std::optional<ScoreRecord> bestScore(
    const std::vector<ScoreRecord>& records, std::string_view difficulty) {

    std::optional<ScoreRecord> best;
    for (const auto& r : records) {
        if (r.difficulty != difficulty) {
            continue;
        }
        if (!best || r.attempts < best->attempts) {
            best = r;
        }
    }
    return best;
}

// 今回の試行回数が、これまでの最高記録を更新したか（記録がなければ更新扱い）
inline bool isNewRecord(const std::optional<ScoreRecord>& previousBest, int attempts) {
    return !previousBest || attempts < previousBest->attempts;
}
