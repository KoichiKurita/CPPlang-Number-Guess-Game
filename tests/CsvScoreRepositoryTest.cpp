#include "infra/CsvScoreRepository.h"
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

namespace fs = std::filesystem;

// テストごとに一時フォルダを作り、終了時に削除する
class CsvScoreRepositoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        const auto* info = ::testing::UnitTest::GetInstance()->current_test_info();
        dir_ = fs::temp_directory_path() / (std::string("ngg_test_") + info->name());
        fs::remove_all(dir_);
        fs::create_directories(dir_);
    }

    void TearDown() override {
        std::error_code ec;
        fs::remove_all(dir_, ec);
    }

    std::string readFile(const fs::path& path) const {
        std::ifstream in(path);
        return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
    }

    void writeFile(const fs::path& path, const std::string& content) const {
        std::ofstream out(path);
        out << content;
    }

    fs::path dir_;
};

TEST_F(CsvScoreRepositoryTest, LoadFromMissingFileReturnsEmpty) {
    const CsvScoreRepository repo(dir_ / "scores.csv");

    EXPECT_TRUE(repo.loadAll().empty());
}

TEST_F(CsvScoreRepositoryTest, SaveWritesHeaderOnceThenAppends) {
    const auto file = dir_ / "scores.csv";
    CsvScoreRepository repo(file);

    repo.save({"Ko1", "Easy", 3});
    repo.save({"Bob", "Hard", 7});

    EXPECT_EQ(readFile(file), "player,difficulty,attempts\nKo1,Easy,3\nBob,Hard,7\n");
}

TEST_F(CsvScoreRepositoryTest, SaveThenLoadRoundTrip) {
    CsvScoreRepository repo(dir_ / "scores.csv");
    repo.save({"Ko1", "Easy", 3});
    repo.save({"John Smith", "Normal", 12});

    const auto records = repo.loadAll();

    ASSERT_EQ(records.size(), 2u);
    EXPECT_EQ(records[0].player, "Ko1");
    EXPECT_EQ(records[0].difficulty, "Easy");
    EXPECT_EQ(records[0].attempts, 3);
    EXPECT_EQ(records[1].player, "John Smith");
    EXPECT_EQ(records[1].attempts, 12);
}

TEST_F(CsvScoreRepositoryTest, CreatesMissingParentDirectories) {
    const auto file = dir_ / "nested" / "deeper" / "scores.csv";
    CsvScoreRepository repo(file);

    repo.save({"Ko1", "Easy", 3});

    EXPECT_TRUE(fs::exists(file));
}

// 手で編集・改ざんされたファイルでも、壊れた行だけを読み飛ばして続行する
TEST_F(CsvScoreRepositoryTest, SkipsMalformedOrInvalidLines) {
    const auto file = dir_ / "scores.csv";
    writeFile(file,
              "player,difficulty,attempts\n"
              "Ko1,Easy,3\n"
              "Easy,8\n"          // 列が足りない（旧形式）
              "Bob,Hard,abc\n"    // 回数が数字ではない
              "Eve,Hard,0\n"      // 回数が範囲外
              "'=cmd,Hard,1\n"    // 名前の検証に通らない
              "\"broken,Easy,1\n" // クォートが閉じていない
              "\n"
              "Carol,Normal,5\r\n"); // CRLF の行

    const auto records = CsvScoreRepository(file).loadAll();

    ASSERT_EQ(records.size(), 2u);
    EXPECT_EQ(records[0].player, "Ko1");
    EXPECT_EQ(records[1].player, "Carol");
    EXPECT_EQ(records[1].attempts, 5);
}

TEST_F(CsvScoreRepositoryTest, SaveFailureThrowsScoreRepositoryError) {
    // 保存先のパスにフォルダがあるため、ファイルとして開けない
    const auto file = dir_ / "scores.csv";
    fs::create_directories(file);
    CsvScoreRepository repo(file);

    EXPECT_THROW(repo.save({"Ko1", "Easy", 3}), ScoreRepositoryError);
}

TEST_F(CsvScoreRepositoryTest, ExposesConfiguredFilePath) {
    const auto file = dir_ / "nested" / "scores.csv";
    const CsvScoreRepository repo(file);

    EXPECT_EQ(repo.file(), file);
}
