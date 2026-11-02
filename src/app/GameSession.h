#pragma once

#include "app/ports/IRandom.h"
#include "app/ports/IScoreRepository.h"
#include "domain/Difficulty.h"
#include "domain/GameEvent.h"
#include "domain/GameState.h"
#include "domain/ScoreRecord.h"

#include <optional>
#include <string>
#include <vector>

// ゲーム終了時のスコア保存の結果
struct ScoreReport {
    std::optional<ScoreRecord> best;    // 表示する最高記録（過去の記録を読めなかった場合は空）
    bool isNewRecord = false;           // 今回のスコアで記録を更新したか
    std::vector<std::string> warnings;  // 保存・読み込みの失敗など、利用者に知らせる警告
};

// 1回の予想の結果
struct GuessOutcome {
    GameEvent event;                    // TooSmall / TooLarge / Correct
    std::optional<ScoreReport> report;  // 正解してゲームが終わったときだけ値を持つ
};

// 1ゲームを「1手ずつ」進めるアプリケーションサービス。
// 入力の待ち方（コンソールのループ / GUI のイベント）に依存しないので、
// Console 版（GameController）と Qt 版の両方から同じルールで使える。
class GameSession {
public:
    GameSession(IRandom& random, IScoreRepository& repo);

    // 新しいゲームを始める（答えを決める）。前のゲームの状態は破棄する。
    void start(std::string player, Difficulty diff);

    // 1回予想する。正解したらスコアを保存し、結果を report に入れて返す。
    // start() の前や、終了後に呼ぶと std::logic_error を投げる。
    GuessOutcome submitGuess(int value);

    bool isPlaying() const;
    const Difficulty& difficulty() const;  // isPlaying() でなくても、最後に始めた難易度を返す
    int attempts() const;

private:
    ScoreReport recordScore(const ScoreRecord& record);

    IRandom& random_;
    IScoreRepository& repo_;
    std::string player_;
    std::optional<GameState> state_;
};
