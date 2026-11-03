#include "infra/Csv.h"

#include <cstddef>
#include <utility>

namespace {

// 表計算ソフトが数式として解釈しうる先頭文字（OWASP: CSV Injection）
bool isFormulaTrigger(char c) {
    return c == '=' || c == '+' || c == '-' || c == '@' || c == '\t' || c == '\r';
}

bool needsQuoting(std::string_view s) {
    return s.find_first_of(",\"\r\n") != std::string_view::npos;
}

// escapeField で付けた「' + 数式トリガー文字」を元に戻す
std::string unescapeFormula(std::string field) {
    if (field.size() >= 2 && field[0] == '\'' && isFormulaTrigger(field[1])) {
        field.erase(0, 1);
    }
    return field;
}

} // namespace

namespace csv {

std::string escapeField(std::string_view field) {
    std::string value;
    if (!field.empty() && isFormulaTrigger(field.front())) {
        value += '\'';
    }
    value += field;

    if (!needsQuoting(value)) {
        return value;
    }

    std::string quoted = "\"";
    for (const char c : value) {
        if (c == '"') {
            quoted += '"'; // " は "" にする
        }
        quoted += c;
    }
    quoted += '"';
    return quoted;
}

std::string formatLine(const std::vector<std::string>& fields) {
    std::string line;
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (i > 0) {
            line += ',';
        }
        line += escapeField(fields[i]);
    }
    return line;
}

std::optional<std::vector<std::string>> parseLine(std::string_view line) {
    std::vector<std::string> fields;
    std::string current;
    bool inQuotes = false;

    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];

        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    current += '"'; // "" は " 1文字
                    ++i;
                } else {
                    inQuotes = false; // クォートの終わり
                }
            } else {
                current += c;
            }
        } else if (c == '"') {
            if (!current.empty()) {
                return std::nullopt; // フィールドの途中に " がある
            }
            inQuotes = true;
        } else if (c == ',') {
            fields.push_back(unescapeFormula(std::move(current)));
            current.clear();
        } else {
            current += c;
        }
    }

    if (inQuotes) {
        return std::nullopt; // クォートが閉じていない
    }
    fields.push_back(unescapeFormula(std::move(current)));
    return fields;
}

} // namespace csv
