#include "domain/Timestamp.h"
#include <gtest/gtest.h>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>

// ---- UNIX 時間 → 文字列 ----
TEST(TimestampTest, FormatsEpoch) {
    EXPECT_EQ(timestamp::format(0), "1970-01-01T00:00:00Z");
}

TEST(TimestampTest, FormatsKnownDates) {
    EXPECT_EQ(timestamp::format(1793716507), "2026-11-03T14:35:07Z");
    EXPECT_EQ(timestamp::format(1709251199), "2024-02-29T23:59:59Z"); // うるう日
    EXPECT_EQ(timestamp::format(951782400), "2000-02-29T00:00:00Z"); // 400 で割り切れる年はうるう年
    EXPECT_EQ(timestamp::format(2147483648), "2038-01-19T03:14:08Z"); // 32 ビットの time_t を超える
}

TEST(TimestampTest, FormatsUpperLimit) {
    EXPECT_EQ(timestamp::format(timestamp::kMaxSeconds), "9999-12-31T23:59:59Z");
}

TEST(TimestampTest, FormatThrowsOutOfRange) {
    EXPECT_THROW(timestamp::format(-1), std::out_of_range);
    EXPECT_THROW(timestamp::format(timestamp::kMaxSeconds + 1), std::out_of_range);
}

// ---- 文字列 → UNIX 時間 ----
TEST(TimestampTest, ParsesValidTimestamps) {
    EXPECT_EQ(timestamp::parse("1970-01-01T00:00:00Z"), std::optional<std::int64_t>(0));
    EXPECT_EQ(timestamp::parse("2026-11-03T14:35:07Z"), std::optional<std::int64_t>(1793716507));
    EXPECT_EQ(timestamp::parse("2024-02-29T23:59:59Z"), std::optional<std::int64_t>(1709251199));
    EXPECT_EQ(timestamp::parse("9999-12-31T23:59:59Z"),
              std::optional<std::int64_t>(timestamp::kMaxSeconds));
}

TEST(TimestampTest, RejectsMalformedText) {
    EXPECT_FALSE(timestamp::parse(""));
    EXPECT_FALSE(timestamp::parse("2026-11-03"));                // 時刻がない
    EXPECT_FALSE(timestamp::parse("2026-11-03T14:35:07"));       // Z（UTC）がない
    EXPECT_FALSE(timestamp::parse("2026-11-03T14:35:07+09:00")); // UTC 以外は受け付けない
    EXPECT_FALSE(timestamp::parse("2026/11/03T14:35:07Z"));      // 区切りが違う
    EXPECT_FALSE(timestamp::parse("2026-11-03 14:35:07Z"));      // T がない
    EXPECT_FALSE(timestamp::parse("2026-1a-03T14:35:07Z"));      // 数字以外
    EXPECT_FALSE(timestamp::parse(" 2026-11-03T14:35:07Z"));     // 前後の空白
    EXPECT_FALSE(timestamp::parse("+026-11-03T14:35:07Z"));      // 符号
}

TEST(TimestampTest, RejectsNonexistentDatesAndTimes) {
    EXPECT_FALSE(timestamp::parse("2026-13-01T00:00:00Z")); // 13 月
    EXPECT_FALSE(timestamp::parse("2026-00-01T00:00:00Z")); // 0 月
    EXPECT_FALSE(timestamp::parse("2026-04-31T00:00:00Z")); // 4 月 31 日
    EXPECT_FALSE(timestamp::parse("2026-11-00T00:00:00Z")); // 0 日
    EXPECT_FALSE(timestamp::parse("2023-02-29T00:00:00Z")); // うるう年ではない
    EXPECT_FALSE(timestamp::parse("2100-02-29T00:00:00Z")); // 100 で割り切れる年はうるう年ではない
    EXPECT_FALSE(timestamp::parse("2026-11-03T24:00:00Z")); // 24 時
    EXPECT_FALSE(timestamp::parse("2026-11-03T14:60:00Z")); // 60 分
    EXPECT_FALSE(timestamp::parse("2026-11-03T14:35:60Z")); // うるう秒は扱わない
    EXPECT_FALSE(timestamp::parse("1969-12-31T23:59:59Z")); // 範囲外（1970 年より前）
}

// 変換して戻すと元の値になる（1 日の途中・月末・年末をまたぐように、半端な間隔で調べる）
TEST(TimestampTest, RoundTrip) {
    for (std::int64_t seconds = 0; seconds <= timestamp::kMaxSeconds; seconds += 999'983) {
        const std::string text = timestamp::format(seconds);
        EXPECT_EQ(timestamp::parse(text), std::optional<std::int64_t>(seconds)) << text;
    }
}
