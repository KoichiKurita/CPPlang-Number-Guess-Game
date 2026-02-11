#pragma once
#include "../src/ui/IGameUI.h"
#include <QObject>
#include <QMessageBox>

class QtGameUI : public QObject, public IGameUI {
    Q_OBJECT
public:
    Difficulty selectDifficulty() override {
        return Difficulty::Normal(); // 簡略
    }

    int inputGuess() override {
        return currentGuess_; // 本来はUI入力
    }

    void handleEvent(const GameEvent& event) override {
        std::visit([&](auto&& e) {
            using T = std::decay_t<decltype(e)>;
            if constexpr (std::is_same_v<T, TooSmall>)
                QMessageBox::information(nullptr, "Result", "Too small");
            else if constexpr (std::is_same_v<T, TooLarge>)
                QMessageBox::information(nullptr, "Result", "Too large");
            else if constexpr (std::is_same_v<T, Correct>)
                QMessageBox::information(
                    nullptr, "Result",
                    QString("Correct! Attempts: %1").arg(e.attempts));
        }, event);
    }

private:
    int currentGuess_{0};
};
