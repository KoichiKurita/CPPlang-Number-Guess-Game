#pragma once

#include "app/ports/IScoreRepository.h"

#include <string>

// スコアを CSV ファイル（scores.csv）に追記する IScoreRepository の実装。
// 保存先の指定やエラー処理はステップ5で改善する。
class CsvScoreRepository : public IScoreRepository {
public:
    void save(const std::string& difficulty, int attempts) override;
};
