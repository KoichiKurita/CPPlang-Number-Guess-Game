#pragma once

#include "domain/Difficulty.h"
#include "domain/StringUtil.h"

#include <charconv>
#include <optional>
#include <string_view>
#include <system_error>
#include <variant>

// 入力エラーの種類（UI側でメッセージを出し分けるために区別する）
enum class InputError {
    Empty,       // 空行・空白のみ
    NotANumber,  // "abc" や "5abc" など、整数として解釈できない
    OutOfRange   // 整数だが範囲外（int の桁あふれも含む）
};

// 成功なら int、失敗なら InputError を持つ
using ParseResult = std::variant<int, InputError>;

// 文字列を [min, max] の範囲の整数に変換する純粋関数（I/O なし）
inline ParseResult parseIntInRange(std::string_view text, int min, int max) {
    const std::string_view s = strutil::trim(text);
    if (s.empty()) {
        return InputError::Empty;
    }

    int value = 0;
    const char* const begin = s.data();
    const char* const end = s.data() + s.size();
    const auto [ptr, ec] = std::from_chars(begin, end, value);

    if (ec == std::errc::result_out_of_range) {
        return InputError::OutOfRange;
    }
    // 変換失敗、または末尾に余分な文字が残っている（例: "5abc"）
    if (ec != std::errc{} || ptr != end) {
        return InputError::NotANumber;
    }
    if (value < min || value > max) {
        return InputError::OutOfRange;
    }
    return value;
}

// 予想値の入力を、難易度の範囲で検証する
inline ParseResult parseGuess(std::string_view text, const Difficulty& diff) {
    return parseIntInRange(text, diff.min, diff.max);
}

// 難易度メニュー（1〜3）の入力を Difficulty に変換する
inline std::optional<Difficulty> parseDifficulty(std::string_view text) {
    const ParseResult r = parseIntInRange(text, 1, 3);
    const int* choice = std::get_if<int>(&r);
    if (choice == nullptr) {
        return std::nullopt;
    }
    switch (*choice) {
        case 1:  return Difficulty::Easy();
        case 2:  return Difficulty::Normal();
        default: return Difficulty::Hard();
    }
}
