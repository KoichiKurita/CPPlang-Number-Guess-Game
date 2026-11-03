#pragma once

#include "app/ports/IScoreRepository.h"

#include <filesystem>
#include <vector>

// スコアを CSV ファイルに追記・読み込みする IScoreRepository の実装。
// 形式: 1行目がヘッダ（player,difficulty,attempts,played_at）、以降1行1件。
// played_at は UTC の ISO 8601 形式（例: 2026-11-03T14:35:07Z）。日時が不明なら空にする。
// 日時の列がない古い形式（v2.4.0 まで）のファイル・行も読み込める。
class CsvScoreRepository : public IScoreRepository {
public:
    explicit CsvScoreRepository(std::filesystem::path file);

    void save(const ScoreRecord& record) override;
    [[nodiscard]] std::vector<ScoreRecord> loadAll() const override;

    [[nodiscard]] const std::filesystem::path& file() const { return file_; }

private:
    std::filesystem::path file_;
};
