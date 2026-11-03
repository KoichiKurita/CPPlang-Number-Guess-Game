#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>

// 日時を「UNIX 時間（1970-01-01T00:00:00Z からの秒数）」と
// 「UTC の ISO 8601 形式の文字列（例: 2026-11-03T14:35:07Z）」の間で変換する純粋関数。
// OS の時刻関数（gmtime など。Windows と Linux で使い方が異なる）を使わず、
// 暦の計算だけで変換するので、どの環境でも同じ結果になり、テストもしやすい。
// 暦の計算は Howard Hinnant 氏のアルゴリズム（パブリックドメイン）による。
namespace timestamp {

// 扱う範囲: 1970-01-01T00:00:00Z 〜 9999-12-31T23:59:59Z（年を 4 桁で表せる範囲）
inline constexpr std::int64_t kMinSeconds = 0;
inline constexpr std::int64_t kMaxSeconds = 253402300799;

namespace detail {

inline constexpr std::int64_t kSecondsPerDay = 86400;

struct CivilDate {
    std::int64_t year;
    int month; // 1〜12
    int day;   // 1〜31
};

// 1970-01-01 からの日数を、年月日に変換する
constexpr CivilDate civilFromDays(std::int64_t days) {
    days += 719468; // 起点を 0000-03-01 に移す（うるう日が年の最後になるよう、3 月始まりで数える）
    const std::int64_t era = (days >= 0 ? days : days - 146096) / 146097; // 400 年単位
    const std::int64_t dayOfEra = days - (era * 146097);                  // 0〜146096
    const std::int64_t yearOfEra =
        (dayOfEra - (dayOfEra / 1460) + (dayOfEra / 36524) - (dayOfEra / 146096)) / 365; // 0〜399
    const std::int64_t dayOfYear =
        dayOfEra - ((365 * yearOfEra) + (yearOfEra / 4) - (yearOfEra / 100));
    const std::int64_t shiftedMonth = ((5 * dayOfYear) + 2) / 153; // 0〜11（0 が 3 月）
    const auto day = static_cast<int>(dayOfYear - (((153 * shiftedMonth) + 2) / 5) + 1);
    const auto month = static_cast<int>(shiftedMonth < 10 ? shiftedMonth + 3 : shiftedMonth - 9);
    const std::int64_t year = yearOfEra + (era * 400) + (month <= 2 ? 1 : 0);
    return {year, month, day};
}

// 年月日を、1970-01-01 からの日数に変換する（civilFromDays の逆）
constexpr std::int64_t daysFromCivil(std::int64_t year, int month, int day) {
    year -= month <= 2 ? 1 : 0;
    const std::int64_t era = (year >= 0 ? year : year - 399) / 400;
    const std::int64_t yearOfEra = year - (era * 400);
    const std::int64_t shiftedMonth = month > 2 ? month - 3 : month + 9;
    const std::int64_t dayOfYear = (((153 * shiftedMonth) + 2) / 5) + day - 1;
    const std::int64_t dayOfEra =
        (yearOfEra * 365) + (yearOfEra / 4) - (yearOfEra / 100) + dayOfYear;
    return (era * 146097) + dayOfEra - 719468;
}

constexpr bool isLeapYear(std::int64_t year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

constexpr int daysInMonth(std::int64_t year, int month) {
    constexpr std::array<int, 12> kDays = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return month == 2 && isLeapYear(year) ? 29 : kDays[static_cast<std::size_t>(month - 1)];
}

// 値を指定した桁数の 10 進数で書く（足りない桁は 0 で埋める）
inline void appendDigits(std::string& out, std::int64_t value, std::size_t width) {
    std::string digits(width, '0');
    for (std::size_t i = width; i > 0; --i) {
        digits[i - 1] = static_cast<char>('0' + (value % 10));
        value /= 10;
    }
    out += digits;
}

// text[pos] から count 文字の数字を読む。数字以外が含まれていれば std::nullopt
inline std::optional<int> readDigits(std::string_view text, std::size_t pos, std::size_t count) {
    int value = 0;
    for (std::size_t i = pos; i < pos + count; ++i) {
        const char c = text[i];
        if (c < '0' || c > '9') {
            return std::nullopt;
        }
        value = (value * 10) + (c - '0');
    }
    return value;
}

} // namespace detail

// UNIX 時間を「YYYY-MM-DDTHH:MM:SSZ」に変換する。範囲外なら std::out_of_range を投げる。
inline std::string format(std::int64_t seconds) {
    if (seconds < kMinSeconds || seconds > kMaxSeconds) {
        throw std::out_of_range("timestamp::format: out of range");
    }
    const std::int64_t days = seconds / detail::kSecondsPerDay;
    const std::int64_t secondsOfDay = seconds % detail::kSecondsPerDay;
    const detail::CivilDate date = detail::civilFromDays(days);

    std::string out;
    out.reserve(20);
    detail::appendDigits(out, date.year, 4);
    out += '-';
    detail::appendDigits(out, date.month, 2);
    out += '-';
    detail::appendDigits(out, date.day, 2);
    out += 'T';
    detail::appendDigits(out, secondsOfDay / 3600, 2);
    out += ':';
    detail::appendDigits(out, (secondsOfDay / 60) % 60, 2);
    out += ':';
    detail::appendDigits(out, secondsOfDay % 60, 2);
    out += 'Z';
    return out;
}

// 「YYYY-MM-DDTHH:MM:SSZ」を UNIX 時間に変換する。
// 形式が違う・存在しない日付（2 月 30 日など）・範囲外なら std::nullopt（外部から来た値を厳密に検証する）。
inline std::optional<std::int64_t> parse(std::string_view text) {
    if (text.size() != 20 || text[4] != '-' || text[7] != '-' || text[10] != 'T' ||
        text[13] != ':' || text[16] != ':' || text[19] != 'Z') {
        return std::nullopt;
    }
    const auto year = detail::readDigits(text, 0, 4);
    const auto month = detail::readDigits(text, 5, 2);
    const auto day = detail::readDigits(text, 8, 2);
    const auto hour = detail::readDigits(text, 11, 2);
    const auto minute = detail::readDigits(text, 14, 2);
    const auto second = detail::readDigits(text, 17, 2);
    if (!year || !month || !day || !hour || !minute || !second) {
        return std::nullopt;
    }
    if (*year < 1970 || *month < 1 || *month > 12 || *day < 1 ||
        *day > detail::daysInMonth(*year, *month) || *hour > 23 || *minute > 59 || *second > 59) {
        return std::nullopt;
    }
    const std::int64_t secondsOfDay =
        (std::int64_t{*hour} * 3600) + (std::int64_t{*minute} * 60) + *second;
    return (detail::daysFromCivil(*year, *month, *day) * detail::kSecondsPerDay) + secondsOfDay;
}

} // namespace timestamp
