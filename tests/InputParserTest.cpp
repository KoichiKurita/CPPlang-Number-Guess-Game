#include "domain/InputParser.h"
#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <variant>

namespace {

// ParseResult を読みやすく検証するためのヘルパー
void expectValue(const ParseResult& r, int expected) {
    ASSERT_TRUE(std::holds_alternative<int>(r));
    EXPECT_EQ(std::get<int>(r), expected);
}

void expectError(const ParseResult& r, InputError expected) {
    ASSERT_TRUE(std::holds_alternative<InputError>(r));
    EXPECT_EQ(std::get<InputError>(r), expected);
}

// parseDifficulty が成功し、期待した難易度になることを確かめる
void expectDifficulty(std::string_view input, const std::string& expectedName) {
    const auto diff = parseDifficulty(input);
    ASSERT_TRUE(diff.has_value()) << "input: " << input;
    EXPECT_EQ(diff->name, expectedName);
}

} // namespace

// ---- 正常系 ----
TEST(InputParserTest, AcceptsNumberInRange) {
    expectValue(parseIntInRange("5", 1, 10), 5);
}

TEST(InputParserTest, AcceptsBoundaryValues) {
    expectValue(parseIntInRange("1", 1, 10), 1);
    expectValue(parseIntInRange("10", 1, 10), 10);
}

TEST(InputParserTest, TrimsSurroundingWhitespace) {
    expectValue(parseIntInRange("  7\t", 1, 10), 7);
    expectValue(parseIntInRange("7\r", 1, 10), 7); // Windows の改行(CRLF)対策
}

// ---- 異常系 ----
TEST(InputParserTest, RejectsEmptyAndBlank) {
    expectError(parseIntInRange("", 1, 10), InputError::Empty);
    expectError(parseIntInRange("   ", 1, 10), InputError::Empty);
}

TEST(InputParserTest, RejectsNonNumeric) {
    expectError(parseIntInRange("abc", 1, 10), InputError::NotANumber);
    expectError(parseIntInRange("5abc", 1, 10), InputError::NotANumber);
    expectError(parseIntInRange("5 5", 1, 10), InputError::NotANumber);
    expectError(parseIntInRange("+5", 1, 10), InputError::NotANumber);
    expectError(parseIntInRange("0x5", 1, 10), InputError::NotANumber);
}

TEST(InputParserTest, RejectsOutOfRange) {
    expectError(parseIntInRange("0", 1, 10), InputError::OutOfRange);
    expectError(parseIntInRange("11", 1, 10), InputError::OutOfRange);
    expectError(parseIntInRange("-3", 1, 10), InputError::OutOfRange);
}

TEST(InputParserTest, RejectsIntOverflow) {
    expectError(parseIntInRange("99999999999999999999", 1, 10), InputError::OutOfRange);
}

// ---- 難易度・予想値 ----
TEST(InputParserTest, ParseGuessUsesDifficultyRange) {
    expectValue(parseGuess("50", Difficulty::Normal()), 50);
    expectError(parseGuess("51", Difficulty::Normal()), InputError::OutOfRange);
}

TEST(InputParserTest, ParseDifficultyMapsMenuChoices) {
    expectDifficulty("1", "Easy");
    expectDifficulty("2", "Normal");
    expectDifficulty("3", "Hard");
}

TEST(InputParserTest, ParseDifficultyRejectsInvalidChoices) {
    EXPECT_FALSE(parseDifficulty("0").has_value());
    EXPECT_FALSE(parseDifficulty("4").has_value());
    EXPECT_FALSE(parseDifficulty("").has_value());
    EXPECT_FALSE(parseDifficulty("easy").has_value());
}
