#include <gtest/gtest.h>
#include "../src/app/GameController.h"
#include "MockRandom.h"
#include "MockUI.h"
#include "../src/infra/IScoreRepository.h"

class DummyScoreRepo : public IScoreRepository {
public:
    void save(const std::string&, int) override {}
};

TEST(GameLogicTest, PureFunction) {
    GameState s{Difficulty::Easy(), 5, 0, false};

    auto [s1, e1] = guess(s, 3);
    EXPECT_FALSE(s1.finished);

    auto [s2, e2] = guess(s1, 5);
    EXPECT_TRUE(s2.finished);
}
