#include "domain/HighScore.h"
#include <gtest/gtest.h>

#include <vector>

TEST(HighScoreTest, NoRecordsMeansNoBest) {
    EXPECT_FALSE(bestScore({}, "Easy").has_value());
}

TEST(HighScoreTest, PicksFewestAttemptsForDifficulty) {
    const std::vector<ScoreRecord> records{
        {"A", "Easy", 5},
        {"B", "Easy", 2},
        {"C", "Hard", 1}, // 別の難易度は対象外
        {"D", "Easy", 4},
    };

    const auto best = bestScore(records, "Easy");

    ASSERT_TRUE(best.has_value());
    EXPECT_EQ(best->player, "B");
    EXPECT_EQ(best->attempts, 2);
}

TEST(HighScoreTest, TieKeepsEarlierRecord) {
    const std::vector<ScoreRecord> records{
        {"First", "Easy", 3},
        {"Second", "Easy", 3},
    };

    EXPECT_EQ(bestScore(records, "Easy")->player, "First");
}

TEST(HighScoreTest, IsNewRecordOnlyWhenStrictlyBetter) {
    const ScoreRecord previous{"A", "Easy", 3};

    EXPECT_TRUE(isNewRecord(std::nullopt, 10)); // 初回は必ず更新
    EXPECT_TRUE(isNewRecord(previous, 2));
    EXPECT_FALSE(isNewRecord(previous, 3)); // 同点は更新しない
    EXPECT_FALSE(isNewRecord(previous, 4));
}
