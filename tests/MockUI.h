#pragma once
#include "app/ports/IGameUI.h"

#include <optional>
#include <queue>
#include <string>
#include <utility>
#include <vector>

// 決められたプレイヤー名・難易度・予想値を順に返す UI。
// 予想値を使い切ったら、EOF と同じく InputClosedError を投げる。
class MockUI : public IGameUI {
public:
    struct BestScoreShown {
        ScoreRecord best;
        bool isNewRecord;
    };

    MockUI(Difficulty diff, std::queue<int> guesses, std::string player = "Ko1")
        : player_(std::move(player)), diff_(std::move(diff)), guesses_(std::move(guesses)) {}

    std::string inputPlayerName() override { return player_; }

    Difficulty selectDifficulty() override { return diff_; }

    int inputGuess(const Difficulty&) override {
        if (guesses_.empty()) {
            throw InputClosedError{};
        }
        const int v = guesses_.front();
        guesses_.pop();
        return v;
    }

    void handleEvent(const GameEvent& event) override { events_.push_back(event); }

    void showBestScore(const ScoreRecord& best, bool isNewRecord) override {
        bestScoreShown_ = BestScoreShown{best, isNewRecord};
    }

    void showWarning(const std::string& message) override { warnings_.push_back(message); }

    const std::vector<GameEvent>& events() const { return events_; }
    const std::optional<BestScoreShown>& bestScoreShown() const { return bestScoreShown_; }
    const std::vector<std::string>& warnings() const { return warnings_; }

private:
    std::string player_;
    Difficulty diff_;
    std::queue<int> guesses_;
    std::vector<GameEvent> events_;
    std::optional<BestScoreShown> bestScoreShown_;
    std::vector<std::string> warnings_;
};
