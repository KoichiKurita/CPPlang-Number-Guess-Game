#include <gtest/gtest.h>
#include "infra/Csv.h"

#include <string>
#include <vector>

// ---- 書き出し ----
TEST(CsvTest, PlainFieldIsUnchanged) {
    EXPECT_EQ(csv::escapeField("Ko1"), "Ko1");
}

TEST(CsvTest, QuotesFieldWithSeparatorQuoteOrNewline) {
    EXPECT_EQ(csv::escapeField("a,b"), "\"a,b\"");
    EXPECT_EQ(csv::escapeField("say \"hi\""), "\"say \"\"hi\"\"\"");
    EXPECT_EQ(csv::escapeField("a\nb"), "\"a\nb\"");
}

TEST(CsvTest, PrefixesFormulaTriggers) {
    EXPECT_EQ(csv::escapeField("=1+1"), "'=1+1");
    EXPECT_EQ(csv::escapeField("+1"), "'+1");
    EXPECT_EQ(csv::escapeField("-1"), "'-1");
    EXPECT_EQ(csv::escapeField("@A1"), "'@A1");
    EXPECT_EQ(csv::escapeField("=HYPERLINK(\"x\",\"y\")"), "\"'=HYPERLINK(\"\"x\"\",\"\"y\"\")\"");
}

TEST(CsvTest, FormatLineJoinsWithComma) {
    EXPECT_EQ(csv::formatLine({"Ko1", "Easy", "3"}), "Ko1,Easy,3");
}

// ---- 読み込み ----
TEST(CsvTest, ParsesSimpleLine) {
    const auto fields = csv::parseLine("Ko1,Easy,3");

    ASSERT_TRUE(fields.has_value());
    EXPECT_EQ(*fields, (std::vector<std::string>{"Ko1", "Easy", "3"}));
}

TEST(CsvTest, ParsesEmptyFields) {
    EXPECT_EQ(*csv::parseLine(",,"), (std::vector<std::string>{"", "", ""}));
}

TEST(CsvTest, RejectsMalformedQuotes) {
    EXPECT_FALSE(csv::parseLine("\"unterminated,Easy,3").has_value());
    EXPECT_FALSE(csv::parseLine("ab\"c,Easy,3").has_value());
}

// 書き出した値を読み込むと元に戻る（ラウンドトリップ）
TEST(CsvTest, RoundTripPreservesValues) {
    const std::vector<std::string> original{
        "plain", "a,b", "say \"hi\"", "=1+1", "-5", "@x", "", "'quoted",
    };

    const auto parsed = csv::parseLine(csv::formatLine(original));

    ASSERT_TRUE(parsed.has_value());
    EXPECT_EQ(*parsed, original);
}
