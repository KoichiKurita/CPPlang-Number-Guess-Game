#include "app/GameController.h"

#include <string>
#include <utility>

GameController::GameController(
    IRandom& random,
    IScoreRepository& repo,
    IGameUI& ui
)
    : session_(random, repo),
      ui_(ui)
{
}

void GameController::run() {
    std::string player = ui_.inputPlayerName();
    session_.start(std::move(player), ui_.selectDifficulty());

    while (session_.isPlaying()) {
        const int value = ui_.inputGuess(session_.difficulty());
        const GuessOutcome outcome = session_.submitGuess(value);
        ui_.handleEvent(outcome.event);

        if (outcome.report) {
            showReport(*outcome.report);
        }
    }
}

void GameController::showReport(const ScoreReport& report) {
    for (const auto& warning : report.warnings) {
        ui_.showWarning(warning);
    }
    if (report.best) {
        ui_.showBestScore(*report.best, report.isNewRecord);
    }
}
