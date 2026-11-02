#pragma once

#include "app/ports/IGameUI.h"
#include "app/ports/IRandom.h"
#include "app/ports/IScoreRepository.h"

// 1ゲームの進行を制御する。具体的な UI・乱数・保存先は知らず、ポート経由で扱う。
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
