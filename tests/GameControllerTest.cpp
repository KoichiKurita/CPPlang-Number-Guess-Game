#include "app/GameController.h"
#include "FakeScoreRepository.h"
#include "FixedClock.h"
#include "MockRandom.h"
#include "MockUI.h"
#include <gtest/gtest.h>

#include <string>
#include <variant>

TEST(GameControllerTest, PlaysUntilCorrectAndSavesScore) {
    MockRandom random(5);
    FakeScoreRepository repo;
    MockUI ui(Difficulty::Easy(), std::queue<int>({3, 8, 5}), "Alice");

    FixedClock clock;
    GameController controller(random, repo, clock, ui);
    controller.run();

    // UI に通知されたイベントの順序
    const auto& events = ui.events();
    ASSERT_EQ(events.size(), 3u);
    EXPECT_TRUE(std::holds_alternative<TooSmall>(events[0]));
    EXPECT_TRUE(std::holds_alternative<TooLarge>(events[1]));
    ASSERT_TRUE(std::holds_alternative<Correct>(events[2]));
    EXPECT_EQ(std::get<Correct>(events[2]).attempts, 3);

    // スコアがプレイヤー名付きで1回だけ保存される
    ASSERT_EQ(repo.records().size(), 1u);
    EXPECT_EQ(repo.records()[0].player, "Alice");
    EXPECT_EQ(repo.records()[0].difficulty, "Easy");
    EXPECT_EQ(repo.records()[0].attempts, 3);
}

TEST(GameControllerTest, GeneratesAnswerWithinSelectedDifficulty) {
    MockRandom random(42);
    FakeScoreRepository repo;
    MockUI ui(Difficulty::Hard(), std::queue<int>({42}));

    FixedClock clock;
    GameController controller(random, repo, clock, ui);
    controller.run();

    EXPECT_EQ(random.callCount(), 1);
    EXPECT_EQ(random.lastMin(), 1);
    EXPECT_EQ(random.lastMax(), 100);
}

TEST(GameControllerTest, InputClosedMidGamePropagatesAndDoesNotSave) {
    MockRandom random(5);
    FakeScoreRepository repo;
    MockUI ui(Difficulty::Easy(), std::queue<int>({1, 2})); // 正解前に入力が尽きる

    FixedClock clock;
    GameController controller(random, repo, clock, ui);

    EXPECT_THROW(controller.run(), InputClosedError);
    EXPECT_TRUE(repo.records().empty());
}

// ---- 最高記録 ----
TEST(GameControllerTest, FirstPlayIsNewRecord) {
    MockRandom random(5);
    FakeScoreRepository repo;
    MockUI ui(Difficulty::Easy(), std::queue<int>({5}));

    FixedClock clock;
    GameController(random, repo, clock, ui).run();

    ASSERT_TRUE(ui.bestScoreShown().has_value());
    EXPECT_TRUE(ui.bestScoreShown()->isNewRecord);
    EXPECT_EQ(ui.bestScoreShown()->best.attempts, 1);
}

TEST(GameControllerTest, ShowsPreviousBestWhenNotBeaten) {
    MockRandom random(5);
    FakeScoreRepository repo({{"Bob", "Easy", 2}});
    MockUI ui(Difficulty::Easy(), std::queue<int>({1, 2, 5})); // 3回で正解

    FixedClock clock;
    GameController(random, repo, clock, ui).run();

    ASSERT_TRUE(ui.bestScoreShown().has_value());
    EXPECT_FALSE(ui.bestScoreShown()->isNewRecord);
    EXPECT_EQ(ui.bestScoreShown()->best.player, "Bob");
    EXPECT_EQ(ui.bestScoreShown()->best.attempts, 2);
}

TEST(GameControllerTest, BestScoreIsPerDifficulty) {
    MockRandom random(5);
    FakeScoreRepository repo({{"Bob", "Hard", 1}}); // 別の難易度の記録
    MockUI ui(Difficulty::Easy(), std::queue<int>({1, 5}));

    FixedClock clock;
    GameController(random, repo, clock, ui).run();

    ASSERT_TRUE(ui.bestScoreShown().has_value());
    EXPECT_TRUE(ui.bestScoreShown()->isNewRecord);
}

// ---- 保存・読み込みの失敗（ゲームは異常終了させない） ----
TEST(GameControllerTest, SaveFailureShowsWarningInsteadOfCrashing) {
    MockRandom random(5);
    FakeScoreRepository repo;
    repo.failOnSave = true;
    MockUI ui(Difficulty::Easy(), std::queue<int>({5}));

    FixedClock clock;
    EXPECT_NO_THROW(GameController(random, repo, clock, ui).run());

    ASSERT_EQ(ui.warnings().size(), 1u);
    EXPECT_NE(ui.warnings()[0].find("Could not save your score"), std::string::npos);
    EXPECT_FALSE(ui.bestScoreShown().has_value());
}

TEST(GameControllerTest, LoadFailureStillSavesButSkipsBestScore) {
    MockRandom random(5);
    FakeScoreRepository repo;
    repo.failOnLoad = true;
    MockUI ui(Difficulty::Easy(), std::queue<int>({5}));

    FixedClock clock;
    EXPECT_NO_THROW(GameController(random, repo, clock, ui).run());

    EXPECT_EQ(repo.records().size(), 1u); // 保存はできている
    ASSERT_EQ(ui.warnings().size(), 1u);
    EXPECT_NE(ui.warnings()[0].find("Could not read past scores"), std::string::npos);
    EXPECT_FALSE(ui.bestScoreShown().has_value()); // 比較できないので表示しない
}
