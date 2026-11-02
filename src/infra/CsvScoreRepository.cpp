#include "infra/CsvScoreRepository.h"

#include "domain/InputParser.h"
#include "domain/PlayerName.h"
#include "infra/Csv.h"

#include <climits>
#include <fstream>
#include <optional>
#include <string>
#include <system_error>
#include <utility>
#include <variant>

namespace {

constexpr const char* kHeader = "player,difficulty,attempts";

// 1行を ScoreRecord に変換する。形式が不正なら std::nullopt（その行は読み飛ばす）。
std::optional<ScoreRecord> toRecord(const std::string& line) {
    const auto fields = csv::parseLine(line);
    if (!fields || fields->size() != 3) {
        return std::nullopt;
    }

    // 読み込み時も検証する（ファイルは手で編集・改ざんされうる外部入力）
    const NameResult name = parsePlayerName((*fields)[0]);
    const ParseResult attempts = parseIntInRange((*fields)[2], 1, INT_MAX);
    if (!std::holds_alternative<std::string>(name) || !std::holds_alternative<int>(attempts)) {
        return std::nullopt;
    }

    return ScoreRecord{ std::get<std::string>(name), (*fields)[1], std::get<int>(attempts) };
}

} // namespace

CsvScoreRepository::CsvScoreRepository(std::filesystem::path file)
    : file_(std::move(file)) {}

void CsvScoreRepository::save(const ScoreRecord& record) {
    namespace fs = std::filesystem;
    std::error_code ec;

    // 保存先のフォルダがなければ作る
    if (file_.has_parent_path()) {
        fs::create_directories(file_.parent_path(), ec);
        if (ec) {
            throw ScoreRepositoryError("cannot create directory '"
                + file_.parent_path().string() + "': " + ec.message());
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
    out << csv::formatLine({ record.player, record.difficulty, std::to_string(record.attempts) })
        << '\n';

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
        return records;  // まだ一度も保存していない
    }

    std::ifstream in(file_);
    if (!in) {
        throw ScoreRepositoryError("cannot open '" + file_.string() + "' for reading");
    }

    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();  // Windows の改行（CRLF）で保存されたファイルにも対応
        }
        if (line.empty() || line == kHeader) {
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
