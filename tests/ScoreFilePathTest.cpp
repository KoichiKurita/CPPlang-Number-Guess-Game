#include "infra/ScoreFilePath.h"
#include <gtest/gtest.h>

#include <cstdlib>
#include <filesystem>
#include <optional>
#include <string>

namespace {

// テストの間だけ環境変数を設定（nullptr なら削除）し、終了時に元の値へ戻す
class ScopedEnv {
public:
    ScopedEnv(const char* name, const char* value) : name_(name) {
        const char* old = std::getenv(name);
        if (old != nullptr) {
            old_ = old;
        }
        set(value);
    }

    ~ScopedEnv() { set(old_.has_value() ? old_->c_str() : nullptr); }

    ScopedEnv(const ScopedEnv&) = delete;
    ScopedEnv& operator=(const ScopedEnv&) = delete;
    ScopedEnv(ScopedEnv&&) = delete;
    ScopedEnv& operator=(ScopedEnv&&) = delete;

private:
    void set(const char* value) const {
#ifdef _WIN32
        _putenv_s(name_, value != nullptr ? value : ""); // 空文字を設定すると削除になる
#else
        if (value != nullptr) {
            setenv(name_, value, 1);
        } else {
            unsetenv(name_);
        }
#endif
    }

    const char* name_;
    std::optional<std::string> old_;
};

} // namespace

TEST(ScoreFilePathTest, EnvironmentVariableOverridesDefault) {
    const ScopedEnv env("NGG_SCORE_FILE", "custom/my-scores.csv");

    EXPECT_EQ(defaultScoreFilePath(), std::filesystem::path("custom/my-scores.csv"));
}

TEST(ScoreFilePathTest, EmptyOverrideIsIgnored) {
    const ScopedEnv env("NGG_SCORE_FILE", "");

    EXPECT_EQ(defaultScoreFilePath().filename(), std::filesystem::path("scores.csv"));
    EXPECT_NE(defaultScoreFilePath(), std::filesystem::path(""));
}

#ifdef _WIN32
TEST(ScoreFilePathTest, UsesAppDataOnWindows) {
    constexpr const char* kAppData = R"(C:\Users\test\AppData\Roaming)";
    const ScopedEnv override("NGG_SCORE_FILE", nullptr);
    const ScopedEnv appData("APPDATA", kAppData);

    EXPECT_EQ(defaultScoreFilePath(),
              std::filesystem::path(kAppData) / "NumberGuessGame" / "scores.csv");
}
#else

TEST(ScoreFilePathTest, UsesXdgDataHome) {
    const ScopedEnv override("NGG_SCORE_FILE", nullptr);
    const ScopedEnv xdg("XDG_DATA_HOME", "/data/xdg");

    EXPECT_EQ(defaultScoreFilePath(),
              std::filesystem::path("/data/xdg/NumberGuessGame/scores.csv"));
}

TEST(ScoreFilePathTest, FallsBackToHomeLocalShare) {
    const ScopedEnv override("NGG_SCORE_FILE", nullptr);
    const ScopedEnv xdg("XDG_DATA_HOME", nullptr);
    const ScopedEnv home("HOME", "/home/test");

    EXPECT_EQ(defaultScoreFilePath(),
              std::filesystem::path("/home/test/.local/share/NumberGuessGame/scores.csv"));
}

TEST(ScoreFilePathTest, FallsBackToCurrentDirectory) {
    const ScopedEnv override("NGG_SCORE_FILE", nullptr);
    const ScopedEnv xdg("XDG_DATA_HOME", nullptr);
    const ScopedEnv home("HOME", nullptr);

    EXPECT_EQ(defaultScoreFilePath(), std::filesystem::path("scores.csv"));
}
#endif
