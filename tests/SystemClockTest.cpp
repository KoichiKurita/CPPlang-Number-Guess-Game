#include "infra/SystemClock.h"
#include <gtest/gtest.h>

#include <cstdint>
#include <ctime>

// SystemClock は「system_clock の起点が UNIX 時間の起点と同じ」という前提に立っている
// （C++17 では規格で保証されていない）。この前提が、実際の環境で成り立つことを確かめる。
TEST(SystemClockTest, ReturnsCurrentUnixTime) {
    const SystemClock clock;

    const auto before = static_cast<std::int64_t>(std::time(nullptr));
    const std::int64_t now = clock.nowUnixSeconds();
    const auto after = static_cast<std::int64_t>(std::time(nullptr));

    // C の time() も UNIX 時間を返す（POSIX と Windows で規定されている）ので、同じ秒になるはず
    EXPECT_GE(now, before);
    EXPECT_LE(now, after);
    // 起点がずれていれば、このプロジェクトを作った年より前の値になる
    EXPECT_GT(now, std::int64_t{1767225600}); // 2026-01-01T00:00:00Z
}
