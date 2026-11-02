#include "app/GameController.h"

#include "domain/GameLogic.h"
#include "domain/GameState.h"
#include "domain/HighScore.h"

#include <optional>
#include <string>

GameController::GameController(
    IRandom& random,
    IScoreRepository& repo,
    IGameUI& ui
)
    : random_(random),
      repo_(repo),
      ui_(ui)
{
}

void GameController::run() {
    const std::string player = ui_.inputPlayerName();
    auto diff = ui_.selectDifficulty();
    int answer = random_.generate(diff.min, diff.max);

    GameState state{ diff, answer, 0, false };

    while (!state.finished) {
        int value = ui_.inputGuess(diff);
        auto [nextState, event] = guess(state, value);
        state = nextState;
        ui_.handleEvent(event);
    }

    recordScore(ScoreRecord{ player, diff.name, state.attempts });
}

void GameController::recordScore(const ScoreRecord& record) {
    // 1. 保存前の最高記録を読む（読めなくても保存は試みる）
    std::optional<ScoreRecord> previousBest;
    bool loaded = false;
    try {
        previousBest = bestScore(repo_.loadAll(), record.difficulty);
        loaded = true;
    }
    catch (const ScoreRepositoryError& e) {
        ui_.showWarning(std::string("Could not read past scores: ") + e.what());
    }

    // 2. 今回のスコアを保存する
    try {
        repo_.save(record);
    }
    catch (const ScoreRepositoryError& e) {
        ui_.showWarning(std::string("Could not save your score: ") + e.what());
        return;
    }

    // 3. 過去の記録を読めたときだけ、最高記録を表示する
    if (loaded) {
        const bool newRecord = isNewRecord(previousBest, record.attempts);
        ui_.showBestScore(newRecord ? record : *previousBest, newRecord);
    }
}
