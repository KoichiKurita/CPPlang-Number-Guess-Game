#include <gtest/gtest.h>
#include "infra/RandomMT.h"

#include <stdexcept>

TEST(RandomMTTest, GeneratesValuesWithinRange) {
    RandomMT random(12345u);

    for (int i = 0; i < 1000; ++i) {
        const int v = random.generate(1, 10);
        EXPECT_GE(v, 1);
        EXPECT_LE(v, 10);
    }
}

TEST(RandomMTTest, SingleValueRangeAlwaysReturnsThatValue) {
    RandomMT random(1u);

    EXPECT_EQ(random.generate(7, 7), 7);
}

TEST(RandomMTTest, SameSeedProducesSameSequence) {
    RandomMT a(42u);
    RandomMT b(42u);

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(a.generate(1, 100), b.generate(1, 100));
    }
}

TEST(RandomMTTest, ThrowsWhenMinIsGreaterThanMax) {
    RandomMT random(1u);

    EXPECT_THROW(random.generate(10, 1), std::invalid_argument);
}
