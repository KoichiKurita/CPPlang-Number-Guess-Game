#pragma once
#include "app/ports/IScoreRepository.h"

#include <utility>
#include <vector>

// メモリ上にスコアを保持するリポジトリ（ファイルには書き込まない）。
// 失敗を再現するためのスイッチを持つ。
class FakeScoreRepository : public IScoreRepository {
public:
    FakeScoreRepository() = default;
    explicit FakeScoreRepository(std::vector<ScoreRecord> initial)
        : records_(std::move(initial)) {}

    void save(const ScoreRecord& record) override {
        if (failOnSave) {
            throw ScoreRepositoryError("disk full");
        }
        records_.push_back(record);
    }

    std::vector<ScoreRecord> loadAll() const override {
        if (failOnLoad) {
            throw ScoreRepositoryError("permission denied");
        }
        return records_;
    }

    const std::vector<ScoreRecord>& records() const { return records_; }

    bool failOnSave = false;
    bool failOnLoad = false;

private:
    std::vector<ScoreRecord> records_;
};
