#pragma once

#include "app/ports/IGameUI.h"

#include <iosfwd>
#include <string>

class ConsoleUI : public IGameUI {
public:
    // 既定は標準入出力。テストでは std::istringstream などを渡せる。
    ConsoleUI();
    ConsoleUI(std::istream& in, std::ostream& out);

    std::string inputPlayerName() override;
    Difficulty selectDifficulty() override;
    int inputGuess(const Difficulty& diff) override;
    void handleEvent(const GameEvent& event) override;
    void showBestScore(const ScoreRecord& best, bool isNewRecord) override;
    void showWarning(const std::string& message) override;

private:
    // 1行読み込む。EOF やストリームエラーなら InputClosedError を投げる。
    std::string readLine();

    std::istream& in_;
    std::ostream& out_;
};
