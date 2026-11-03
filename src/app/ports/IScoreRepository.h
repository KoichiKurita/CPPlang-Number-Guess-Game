#pragma once

#include "domain/ScoreRecord.h"

#include <stdexcept>
#include <vector>

// スコアの保存・読み込みに失敗したことを表す例外（ファイルを開けない、書き込めない等）。
// 実装（infra）はこの型で失敗を通知し、app はこの型だけを捕捉する。
class ScoreRepositoryError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// スコア保存のポート（app が外部に求める機能）。実装は infra/CsvScoreRepository。
class IScoreRepository {
public:
    virtual ~IScoreRepository() = default;

    // 1件追記する。失敗したら ScoreRepositoryError を投げる。
    virtual void save(const ScoreRecord& record) = 0;

    // 全件を読み込む。保存先がまだなければ空を返す。
    // 読み込めない場合は ScoreRepositoryError を投げる。壊れた行は読み飛ばす。
    [[nodiscard]] virtual std::vector<ScoreRecord> loadAll() const = 0;
};
