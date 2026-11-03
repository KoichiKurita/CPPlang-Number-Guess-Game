#pragma once

#include "app/GameSession.h"
#include "app/ports/IGameUI.h"
#include "app/ports/IRandom.h"
#include "app/ports/IScoreRepository.h"

// Console 版のゲーム進行。IGameUI の「入力を待って返る」形のメソッドでループを回し、
// ルールの適用とスコアの保存は GameSession に任せる。
class GameController {
public:
    GameController(IRandom& random, IScoreRepository& repo, IGameUI& ui);

    void run();

private:
    void showReport(const ScoreReport& report);

    GameSession session_;
    IGameUI& ui_;
};
