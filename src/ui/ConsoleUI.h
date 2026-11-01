#pragma once

#include "IGameUI.h"

#include <iosfwd>
#include <string>

class ConsoleUI : public IGameUI {
public:
    // 既定は標準入出力。テストでは std::istringstream などを渡せる。
    ConsoleUI();
    ConsoleUI(std::istream& in, std::ostream& out);

    Difficulty selectDifficulty() override;
    int inputGuess(const Difficulty& diff) override;
    void handleEvent(const GameEvent& event) override;

private:
    // 1行読み込む。EOF やストリームエラーなら InputClosedError を投げる。
    std::string readLine();

    std::istream& in_;
    std::ostream& out_;
};
