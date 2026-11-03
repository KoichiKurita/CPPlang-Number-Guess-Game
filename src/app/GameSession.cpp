#include "app/GameSession.h"

#include "domain/GameLogic.h"
#include "domain/HighScore.h"

#include <stdexcept>
#include <utility>

GameSession::GameSession(IRandom& random, IScoreRepository& repo, IClock& clock)
    : random_(random), repo_(repo), clock_(clock) {}

void GameSession::start(std::string player, Difficulty diff) {
    const int answer = random_.generate(diff.min, diff.max);
    player_ = std::move(player);
    state_ = GameState{std::move(diff), answer, 0, false};
}

GuessOutcome GameSession::submitGuess(int value) {
    // isPlaying() と同じ条件だが、state_ が値を持つことをこの場で確かめてから使う
    if (!state_ || state_->finished) {
        throw std::logic_error("GameSession::submitGuess: no game in progress");
    }

    auto [next, event] = guess(*state_, value);
    state_ = next;

    GuessOutcome outcome{event, std::nullopt};
    if (state_->finished) {
        outcome.report = recordScore(
            ScoreRecord{player_, state_->diff.name, state_->attempts, clock_.nowUnixSeconds()});
    }
    return outcome;
}

bool GameSession::isPlaying() const {
    return state_.has_value() && !state_->finished;
}

const Difficulty& GameSession::difficulty() const {
    if (!state_) {
        throw std::logic_error("GameSession::difficulty: game has not started");
    }
    return state_->diff;
}

int GameSession::attempts() const {
    return state_ ? state_->attempts : 0;
}

ScoreReport GameSession::recordScore(const ScoreRecord& record) {
    ScoreReport report;

    // 1. 保存前の最高記録を読む（読めなくても保存は試みる）
    std::optional<ScoreRecord> previousBest;
    bool loaded = false;
    try {
        previousBest = bestScore(repo_.loadAll(), record.difficulty);
        loaded = true;
    } catch (const ScoreRepositoryError& e) {
        report.warnings.push_back(std::string("Could not read past scores: ") + e.what());
    }

    // 2. 今回のスコアを保存する
    try {
        repo_.save(record);
    } catch (const ScoreRepositoryError& e) {
        report.warnings.push_back(std::string("Could not save your score: ") + e.what());
        return report;
    }

    // 3. 過去の記録を読めたときだけ、最高記録を返す
    if (loaded) {
        report.isNewRecord = isNewRecord(previousBest, record.attempts);
        report.best = report.isNewRecord ? std::optional<ScoreRecord>(record) : previousBest;
    }
    return report;
}
