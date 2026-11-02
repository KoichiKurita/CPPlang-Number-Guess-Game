#include <gtest/gtest.h>
#include "domain/GameLogic.h"

#include <variant>

namespace {

GameState newGame(int answer) {
    return GameState{Difficulty::Easy(), answer, 0, false};
}

} // namespace

TEST(GameLogicTest, TooSmallWhenGuessIsBelowAnswer) {
    auto [next, event] = guess(newGame(5), 3);

    EXPECT_TRUE(std::holds_alternative<TooSmall>(event));
    EXPECT_EQ(next.attempts, 1);
    EXPECT_FALSE(next.finished);
}

TEST(GameLogicTest, TooLargeWhenGuessIsAboveAnswer) {
    auto [next, event] = guess(newGame(5), 8);

    EXPECT_TRUE(std::holds_alternative<TooLarge>(event));
    EXPECT_EQ(next.attempts, 1);
    EXPECT_FALSE(next.finished);
}

TEST(GameLogicTest, CorrectFinishesGameAndReportsAttempts) {
    auto [s1, e1] = guess(newGame(5), 3);
    auto [s2, e2] = guess(s1, 5);

    ASSERT_TRUE(std::holds_alternative<Correct>(e2));
    EXPECT_EQ(std::get<Correct>(e2).attempts, 2);
    EXPECT_TRUE(s2.finished);
}

TEST(GameLogicTest, DoesNotMutateInputState) {
    const GameState before = newGame(5);
    guess(before, 3);

    EXPECT_EQ(before.attempts, 0);   // 純粋関数: 引数は変更されない
    EXPECT_FALSE(before.finished);
}

TEST(GameLogicTest, GuessAfterFinishedDoesNotCountAttempt) {
    auto [finished, e1] = guess(newGame(5), 5);
    auto [again, e2] = guess(finished, 1);

    EXPECT_EQ(again.attempts, 1);
    ASSERT_TRUE(std::holds_alternative<Correct>(e2));
    EXPECT_EQ(std::get<Correct>(e2).attempts, 1);
}
