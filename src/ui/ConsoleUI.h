#pragma once

#include "IGameUI.h"
#include <iostream>
#include <variant>        // ★必須
#include <type_traits>    // ★必須

class ConsoleUI : public IGameUI {
public:
    Difficulty selectDifficulty() override {
        std::cout << "1: Easy  2: Normal  3: Hard\n";
        int c;
        std::cin >> c;

        return c == 1 ? Difficulty::Easy()
             : c == 2 ? Difficulty::Normal()
                      : Difficulty::Hard();
    }

    int inputGuess() override {
        std::cout << "Guess: ";
        int v;
        std::cin >> v;
        return v;
    }

    void handleEvent(const GameEvent& event) override {
        std::visit([&](auto&& e) {
            using T = std::decay_t<decltype(e)>;

            if constexpr (std::is_same_v<T, TooSmall>) {
                std::cout << "Too small\n";
            }
            else if constexpr (std::is_same_v<T, TooLarge>) {
                std::cout << "Too large\n";
            }
            else if constexpr (std::is_same_v<T, Correct>) {
                std::cout << "Correct! Attempts: "
                          << e.attempts << "\n";
            }
        }, event);
    }
};
