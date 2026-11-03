#include "infra/CsvScoreRepository.h"

#include "domain/InputParser.h"
#include "domain/PlayerName.h"
#include "domain/Timestamp.h"
#include "infra/Csv.h"

#include <climits>
#include <cstdint>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <variant>
#include <vector>

namespace {

// 現在の形式のヘッダ（日時の列 played_at は v2.5.0 で追加した）
constexpr const char* kHeader = "player,difficulty,attempts,played_at";
// v2.4.0 までの形式のヘッダ。古いファイルに追記した場合は、このヘッダのまま残る
constexpr const char* kLegacyHeader = "player,difficulty,attempts";

// 日時を書き出す文字列にする。日時がない・扱える範囲外（PC の時計が大きくずれている）なら空にする
std::string formatPlayedAt(const std::optional<std::int64_t>& playedAt) {
    if (!playedAt || *playedAt < timestamp::kMinSeconds || *playedAt > timestamp::kMaxSeconds) {
        return {};
    }
    return timestamp::format(*playedAt);
}

// 1行を ScoreRecord に変換する。形式が不正なら std::nullopt（その行は読み飛ばす）。
std::optional<ScoreRecord> toRecord(const std::string& line) {
    const auto fields = csv::parseLine(line);
    // 列は 4 つ（現在の形式）か 3 つ（日時の列がない古い形式）
    if (!fields || (fields->size() != 4 && fields->size() != 3)) {
        return std::nullopt;
    }

    // 読み込み時も検証する（ファイルは手で編集・改ざんされうる外部入力）
    const NameResult name = parsePlayerName((*fields)[0]);
    const ParseResult attempts = parseIntInRange((*fields)[2], 1, INT_MAX);
    if (!std::holds_alternative<std::string>(name) || !std::holds_alternative<int>(attempts)) {
        return std::nullopt;
    }

    // 日時は空なら「不明」。値があるのに形式が不正なら、壊れた行として読み飛ばす
    std::optional<std::int64_t> playedAt;
    if (fields->size() == 4 && !(*fields)[3].empty()) {
        playedAt = timestamp::parse((*fields)[3]);
        if (!playedAt) {
            return std::nullopt;
        }
    }

    return ScoreRecord{
        std::get<std::string>(name),
        (*fields)[1],
        std::get<int>(attempts),
        playedAt,
    };
}

} // namespace

CsvScoreRepository::CsvScoreRepository(std::filesystem::path file) : file_(std::move(file)) {}

void CsvScoreRepository::save(const ScoreRecord& record) {
    namespace fs = std::filesystem;
    std::error_code ec;

    // 保存先のフォルダがなければ作る
    if (file_.has_parent_path()) {
        fs::create_directories(file_.parent_path(), ec);
        if (ec) {
            throw ScoreRepositoryError("cannot create directory '" + file_.parent_path().string() +
                                       "': " + ec.message());
        }
    }

    // 新規ファイル（または空ファイル）ならヘッダを書く
    const bool needsHeader = !fs::exists(file_, ec) || fs::file_size(file_, ec) == 0;

    std::ofstream out(file_, std::ios::app);
    if (!out) {
        throw ScoreRepositoryError("cannot open '" + file_.string() + "' for writing");
    }
    if (needsHeader) {
        out << kHeader << '\n';
    }
    const std::vector<std::string> fields = {
        record.player,
        record.difficulty,
        std::to_string(record.attempts),
        formatPlayedAt(record.playedAt),
    };
    out << csv::formatLine(fields) << '\n';

    // ディスクフルなどの書き込み失敗を検出する
    out.flush();
    if (!out) {
        throw ScoreRepositoryError("failed to write to '" + file_.string() + "'");
    }
}

std::vector<ScoreRecord> CsvScoreRepository::loadAll() const {
    std::vector<ScoreRecord> records;

    std::error_code ec;
    if (!std::filesystem::exists(file_, ec)) {
        return records; // まだ一度も保存していない
    }

    std::ifstream in(file_);
    if (!in) {
        throw ScoreRepositoryError("cannot open '" + file_.string() + "' for reading");
    }

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back(); // Windows の改行（CRLF）で保存されたファイルにも対応
        }
        if (line.empty() || line == kHeader || line == kLegacyHeader) {
            continue;
        }
        if (auto record = toRecord(line)) {
            records.push_back(std::move(*record));
        }
    }

    if (in.bad()) {
        throw ScoreRepositoryError("failed to read '" + file_.string() + "'");
    }
    return records;
}
