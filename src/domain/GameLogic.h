#pragma once
#include "GameState.h"
#include "GameEvent.h"
#include <utility>

inline std::pair<GameState, GameEvent>
guess(const GameState& state, int value) {

    if (state.finished) {
        return { state, Correct{state.attempts} };
    }

    GameState next = state;
    next.attempts++;

    if (value < state.answer) {
        return { next, TooSmall{} };
    }

    if (value > state.answer) {
        return { next, TooLarge{} };
    }

    next.finished = true;
    return { next, Correct{next.attempts} };
}
