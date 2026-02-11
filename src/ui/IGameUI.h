#pragma once
#include "../domain/Difficulty.h"
#include "../domain/GuessResult.h"
#include "../domain/GameEvent.h"

class IGameUI {
public:
    virtual ~IGameUI() = default;

    virtual Difficulty selectDifficulty() = 0;
    virtual int inputGuess() = 0;

    // ★ GameEvent を扱う
    virtual void handleEvent(const GameEvent& event) = 0;
};
