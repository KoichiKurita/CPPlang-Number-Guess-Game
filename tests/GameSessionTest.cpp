#include <gtest/gtest.h>
#include "app/GameSession.h"
#include "FakeScoreRepository.h"
#include "MockRandom.h"

#include <stdexcept>
#include <variant>

TEST(GameSessionTest, NotPlayingBeforeStart) {
    MockRandom random(5);
    FakeScoreRepository repo;
    GameSession session(random, repo);

    EXPECT_FALSE(session.isPlaying());
    EXPECT_EQ(session.attempts(), 0);
    EXPECT_THROW(session.submitGuess(5), std::logic_error);
    EXPECT_THROW(session.difficulty(), std::logic_error);
}

TEST(GameSessionTest, StartGeneratesAnswerInDifficultyRange) {
    MockRandom random(30);
    FakeScoreRepository repo;
    GameSession session(random, repo);

    session.start("Ko1", Difficulty::Normal());

    EXPECT_TRUE(session.isPlaying());
    EXPECT_EQ(session.difficulty().name, "Normal");
    EXPECT_EQ(random.lastMin(), 1);
    EXPECT_EQ(random.lastMax(), 50);
}

TEST(GameSessionTest, WrongGuessReturnsHintWithoutReport) {
    MockRandom random(5);
    FakeScoreRepository repo;
    GameSession session(random, repo);
    session.start("Ko1", Difficulty::Easy());

    const GuessOutcome outcome = session.submitGuess(3);

    EXPECT_TRUE(std::holds_alternative<TooSmall>(outcome.event));
    EXPECT_FALSE(outcome.report.has_value());
    EXPECT_TRUE(session.isPlaying());
    EXPECT_EQ(session.attempts(), 1);
    EXPECT_TRUE(repo.records().empty());   // まだ保存しない
}

TEST(GameSessionTest, CorrectGuessFinishesAndSavesScore) {
    MockRandom random(5);
    FakeScoreRepository repo;
    GameSession session(random, repo);
    session.start("Ko1", Difficulty::Easy());
    session.submitGuess(8);

    const GuessOutcome outcome = session.submitGuess(5);

    ASSERT_TRUE(std::holds_alternative<Correct>(outcome.event));
    EXPECT_EQ(std::get<Correct>(outcome.event).attempts, 2);
    EXPECT_FALSE(session.isPlaying());

    ASSERT_TRUE(outcome.report.has_value());
    EXPECT_TRUE(outcome.report->isNewRecord);
    ASSERT_TRUE(outcome.report->best.has_value());
    EXPECT_EQ(outcome.report->best->player, "Ko1");
    EXPECT_TRUE(outcome.report->warnings.empty());

    ASSERT_EQ(repo.records().size(), 1u);
    EXPECT_EQ(repo.records()[0].attempts, 2);
}

TEST(GameSessionTest, GuessAfterFinishThrows) {
    MockRandom random(5);
    FakeScoreRepository repo;
    GameSession session(random, repo);
    session.start("Ko1", Difficulty::Easy());
    session.submitGuess(5);

    EXPECT_THROW(session.submitGuess(5), std::logic_error);
    EXPECT_EQ(repo.records().size(), 1u);  // 二重に保存されない
}

TEST(GameSessionTest, RestartResetsAttempts) {
    MockRandom random(5);
    FakeScoreRepository repo;
    GameSession session(random, repo);
    session.start("Ko1", Difficulty::Easy());
    session.submitGuess(1);
    session.submitGuess(2);

    session.start("Bob", Difficulty::Hard());   // 途中で新しいゲームを始める

    EXPECT_TRUE(session.isPlaying());
    EXPECT_EQ(session.attempts(), 0);
    EXPECT_EQ(session.difficulty().name, "Hard");
}

TEST(GameSessionTest, ReportContainsWarningsWhenSaveFails) {
    MockRandom random(5);
    FakeScoreRepository repo;
    repo.failOnSave = true;
    GameSession session(random, repo);
    session.start("Ko1", Difficulty::Easy());

    const GuessOutcome outcome = session.submitGuess(5);

    ASSERT_TRUE(outcome.report.has_value());
    ASSERT_EQ(outcome.report->warnings.size(), 1u);
    EXPECT_FALSE(outcome.report->best.has_value());
}
