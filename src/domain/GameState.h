#pragma once
#include "Difficulty.h"

struct GameState {
    Difficulty diff;
    int answer;
    int attempts;
    bool finished;
};
