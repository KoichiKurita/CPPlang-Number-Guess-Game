#pragma once
#include "../src/ui/IGameUI.h"

#include <queue>
#include <utility>
#include <vector>

// 決められた難易度と予想値を順に返す UI。
// 予想値を使い切ったら、EOF と同じく InputClosedError を投げる。
class MockUI : public IGameUI {
public:
    MockUI(Difficulty diff, std::queue<int> guesses)
        : diff_(std::move(diff)), guesses_(std::move(guesses)) {}

    Difficulty selectDifficulty() override {
        return diff_;
    }

    int inputGuess(const Difficulty&) override {
        if (guesses_.empty()) {
            throw InputClosedError{};
        }
        const int v = guesses_.front();
        guesses_.pop();
        return v;
    }

    void handleEvent(const GameEvent& event) override {
        events_.push_back(event);
    }

    const std::vector<GameEvent>& events() const { return events_; }

private:
    Difficulty diff_;
    std::queue<int> guesses_;
    std::vector<GameEvent> events_;
};
