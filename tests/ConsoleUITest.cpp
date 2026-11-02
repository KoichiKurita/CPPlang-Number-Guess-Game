#include <gtest/gtest.h>
#include "ui/ConsoleUI.h"

#include <sstream>
#include <string>

// std::istringstream / std::ostringstream を注入して ConsoleUI を検証する
class ConsoleUITest : public ::testing::Test {
protected:
    void setInput(const std::string& text) { in_.str(text); }
    std::string output() const { return out_.str(); }

    std::istringstream in_;
    std::ostringstream out_;
    ConsoleUI ui_{in_, out_};
};

TEST_F(ConsoleUITest, SelectDifficultyRetriesUntilValid) {
    setInput("9\n\nabc\n2\n");

    EXPECT_EQ(ui_.selectDifficulty().name, "Normal");
    EXPECT_NE(output().find("Please choose 1, 2 or 3."), std::string::npos);
}

TEST_F(ConsoleUITest, InputGuessRetriesOnInvalidInput) {
    setInput("abc\n5abc\n0\n\n7\n");

    EXPECT_EQ(ui_.inputGuess(Difficulty::Easy()), 7);

    const std::string out = output();
    EXPECT_NE(out.find("Invalid input. Digits only."), std::string::npos);
    EXPECT_NE(out.find("Out of range."), std::string::npos);
    EXPECT_NE(out.find("Please enter a number."), std::string::npos);
}

TEST_F(ConsoleUITest, InputGuessShowsRangeInPrompt) {
    setInput("10\n");
    ui_.inputGuess(Difficulty::Normal());

    EXPECT_NE(output().find("Guess (1-50): "), std::string::npos);
}

// 修正前は無限ループしていたケースの回帰テスト
TEST_F(ConsoleUITest, ThrowsInputClosedOnEof) {
    setInput("abc\n");   // 不正入力の後に EOF

    EXPECT_THROW(ui_.inputGuess(Difficulty::Easy()), InputClosedError);
}

TEST_F(ConsoleUITest, RejectsTooLongLine) {
    setInput(std::string(1000, '1') + "\n3\n");

    EXPECT_EQ(ui_.inputGuess(Difficulty::Easy()), 3);
    EXPECT_NE(output().find("Please enter a number."), std::string::npos);
}

TEST_F(ConsoleUITest, HandleEventPrintsMessages) {
    ui_.handleEvent(TooSmall{});
    ui_.handleEvent(TooLarge{});
    ui_.handleEvent(Correct{4});

    EXPECT_EQ(output(), "Too small\nToo large\nCorrect! Attempts: 4\n");
}
