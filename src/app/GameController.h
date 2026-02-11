#pragma once

#include "../domain/GameState.h"
#include "../domain/GameEvent.h"
#include "../domain/GameLogic.h"
#include "../infra/IRandom.h"
#include "../infra/IScoreRepository.h"
#include "../ui/IGameUI.h"

class GameController {
public:
    GameController(
        IRandom& random,
        IScoreRepository& repo,
        IGameUI& ui
    );

    void run();

private:
    IRandom& random_;
    IScoreRepository& repo_;
    IGameUI& ui_;
};
