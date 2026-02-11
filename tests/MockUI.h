#pragma once
#include "../src/ui/IGameUI.h"
#include <queue>

class MockUI : public IGameUI {
public:
    explicit MockUI(Difficulty diff, std::queue<int> guesses)
        : diff_(diff), guesses_(guesses), resultShown_(false) {}

    Difficulty selectDifficulty() override {
        return diff_;
    }

    int inputGuess() override {
        int v = guesses_.front();
        guesses_.pop();
        return v;
    }

    void showResult(int attempts) override {
        attempts_ = attempts;
        resultShown_ = true;
    }

    bool resultShown() const { return resultShown_; }
    int attempts() const { return attempts_; }

private:
    Difficulty diff_;
    std::queue<int> guesses_;
    bool resultShown_;
    int attempts_;
};
