#pragma once

#include "app/ports/IScoreRepository.h"

#include <filesystem>
#include <vector>

// スコアを CSV ファイルに追記・読み込みする IScoreRepository の実装。
// 形式: 1行目がヘッダ（player,difficulty,attempts）、以降1行1件。
class CsvScoreRepository : public IScoreRepository {
public:
    explicit CsvScoreRepository(std::filesystem::path file);

    void save(const ScoreRecord& record) override;
    [[nodiscard]] std::vector<ScoreRecord> loadAll() const override;

    [[nodiscard]] const std::filesystem::path& file() const { return file_; }

private:
    std::filesystem::path file_;
};
