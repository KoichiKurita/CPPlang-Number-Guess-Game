#pragma once
#include <string>

// スコア保存のポート（app が外部に求める機能）。実装は infra/CsvScoreRepository。
class IScoreRepository {
public:
    virtual ~IScoreRepository() = default;
    virtual void save(const std::string& difficulty, int attempts) = 0;
};
