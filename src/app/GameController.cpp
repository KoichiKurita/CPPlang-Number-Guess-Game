#include "GameController.h"

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
    auto diff = ui_.selectDifficulty();
    int answer = random_.generate(diff.min, diff.max);

    GameState state{ diff, answer, 0, false };

    while (!state.finished) {
        int value = ui_.inputGuess();
        auto [nextState, event] = guess(state, value);
        state = nextState;
        ui_.handleEvent(event);
    }

    repo_.save(diff.name, state.attempts);
}
