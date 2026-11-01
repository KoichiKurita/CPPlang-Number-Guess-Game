#include <gtest/gtest.h>
#include "../src/app/GameController.h"
#include "MockRandom.h"
#include "MockUI.h"
#include "SpyScoreRepository.h"

#include <variant>

TEST(GameControllerTest, PlaysUntilCorrectAndSavesScore) {
    MockRandom random(5);
    SpyScoreRepository repo;
    MockUI ui(Difficulty::Easy(), std::queue<int>({3, 8, 5}));

    GameController controller(random, repo, ui);
    controller.run();

    // UI に通知されたイベントの順序
    const auto& events = ui.events();
    ASSERT_EQ(events.size(), 3u);
    EXPECT_TRUE(std::holds_alternative<TooSmall>(events[0]));
    EXPECT_TRUE(std::holds_alternative<TooLarge>(events[1]));
    ASSERT_TRUE(std::holds_alternative<Correct>(events[2]));
    EXPECT_EQ(std::get<Correct>(events[2]).attempts, 3);

    // スコアが1回だけ保存される
    ASSERT_EQ(repo.records().size(), 1u);
    EXPECT_EQ(repo.records()[0].difficulty, "Easy");
    EXPECT_EQ(repo.records()[0].attempts, 3);
}

TEST(GameControllerTest, GeneratesAnswerWithinSelectedDifficulty) {
    MockRandom random(42);
    SpyScoreRepository repo;
    MockUI ui(Difficulty::Hard(), std::queue<int>({42}));

    GameController controller(random, repo, ui);
    controller.run();

    EXPECT_EQ(random.callCount(), 1);
    EXPECT_EQ(random.lastMin(), 1);
    EXPECT_EQ(random.lastMax(), 100);
}

TEST(GameControllerTest, InputClosedMidGamePropagatesAndDoesNotSave) {
    MockRandom random(5);
    SpyScoreRepository repo;
    MockUI ui(Difficulty::Easy(), std::queue<int>({1, 2}));  // 正解前に入力が尽きる

    GameController controller(random, repo, ui);

    EXPECT_THROW(controller.run(), InputClosedError);
    EXPECT_TRUE(repo.records().empty());
}
