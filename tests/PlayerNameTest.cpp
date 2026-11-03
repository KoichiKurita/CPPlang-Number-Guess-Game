#include "domain/PlayerName.h"
#include <gtest/gtest.h>

#include <string>
#include <variant>

namespace {

void expectName(const NameResult& r, const std::string& expected) {
    ASSERT_TRUE(std::holds_alternative<std::string>(r));
    EXPECT_EQ(std::get<std::string>(r), expected);
}

void expectError(const NameResult& r, NameError expected) {
    ASSERT_TRUE(std::holds_alternative<NameError>(r));
    EXPECT_EQ(std::get<NameError>(r), expected);
}

} // namespace

TEST(PlayerNameTest, AcceptsAllowedCharacters) {
    expectName(parsePlayerName("Ko1"), "Ko1");
    expectName(parsePlayerName("player_1"), "player_1");
    expectName(parsePlayerName("John Smith-2"), "John Smith-2");
}

TEST(PlayerNameTest, TrimsSurroundingWhitespace) {
    expectName(parsePlayerName("  Ko1 \r"), "Ko1");
}

TEST(PlayerNameTest, AcceptsMaxLengthAndRejectsLonger) {
    expectName(parsePlayerName(std::string(kMaxPlayerNameLength, 'a')),
               std::string(kMaxPlayerNameLength, 'a'));
    expectError(parsePlayerName(std::string(kMaxPlayerNameLength + 1, 'a')), NameError::TooLong);
}

TEST(PlayerNameTest, RejectsEmpty) {
    expectError(parsePlayerName(""), NameError::Empty);
    expectError(parsePlayerName("   "), NameError::Empty);
}

// CSV を表計算ソフトで開いたときに数式として実行されうる値（CSV インジェクション）
TEST(PlayerNameTest, RejectsFormulaInjectionPrefixes) {
    expectError(parsePlayerName("=1+1"), NameError::InvalidStart);
    expectError(parsePlayerName("+cmd"), NameError::InvalidStart);
    expectError(parsePlayerName("-2"), NameError::InvalidStart);
    expectError(parsePlayerName("@SUM(A1)"), NameError::InvalidStart);
}

TEST(PlayerNameTest, RejectsCharactersOutsideAllowList) {
    expectError(parsePlayerName("a,b"), NameError::InvalidCharacter);  // CSV の区切り
    expectError(parsePlayerName("a\"b"), NameError::InvalidCharacter); // CSV のクォート
    expectError(parsePlayerName("a\tb"), NameError::InvalidCharacter);
    expectError(parsePlayerName("a;b"), NameError::InvalidCharacter);
    expectError(parsePlayerName("\xE3\x81\x82"), NameError::InvalidStart); // 非 ASCII（「あ」）
}
