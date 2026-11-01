#pragma once
#include "../src/infra/IScoreRepository.h"

#include <string>
#include <vector>

// save() の呼び出しを記録するだけのリポジトリ（ファイルには書き込まない）
class SpyScoreRepository : public IScoreRepository {
public:
    struct Record {
        std::string difficulty;
        int attempts;
    };

    void save(const std::string& difficulty, int attempts) override {
        records_.push_back({difficulty, attempts});
    }

    const std::vector<Record>& records() const { return records_; }

private:
    std::vector<Record> records_;
};
