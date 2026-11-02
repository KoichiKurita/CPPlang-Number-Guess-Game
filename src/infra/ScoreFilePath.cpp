#include "infra/ScoreFilePath.h"

#include <cstdlib>
#include <optional>
#include <string>

namespace {

constexpr const char* kAppDirName = "NumberGuessGame";
constexpr const char* kFileName = "scores.csv";

// 環境変数を読む。未設定・空なら std::nullopt。
std::optional<std::string> getEnv(const char* name) {
#ifdef _MSC_VER
    // MSVC では getenv が非推奨警告（C4996）になるため、安全版の _dupenv_s を使う
    char* buffer = nullptr;
    std::size_t length = 0;
    if (_dupenv_s(&buffer, &length, name) != 0 || buffer == nullptr) {
        return std::nullopt;
    }
    std::string value(buffer);
    std::free(buffer);
#else
    const char* raw = std::getenv(name);
    if (raw == nullptr) {
        return std::nullopt;
    }
    std::string value(raw);
#endif
    if (value.empty()) {
        return std::nullopt;
    }
    return value;
}

} // namespace

std::filesystem::path defaultScoreFilePath() {
    namespace fs = std::filesystem;

    if (auto overridePath = getEnv("NGG_SCORE_FILE")) {
        return fs::path(*overridePath);
    }

#ifdef _WIN32
    if (auto appData = getEnv("APPDATA")) {
        return fs::path(*appData) / kAppDirName / kFileName;
    }
#else
    if (auto xdg = getEnv("XDG_DATA_HOME")) {
        return fs::path(*xdg) / kAppDirName / kFileName;
    }
    if (auto home = getEnv("HOME")) {
        return fs::path(*home) / ".local" / "share" / kAppDirName / kFileName;
    }
#endif
    return fs::path(kFileName);
}
