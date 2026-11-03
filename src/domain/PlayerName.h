#pragma once

#include "domain/StringUtil.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <variant>

// プレイヤー名の入力エラー
enum class NameError : std::uint8_t {
    Empty,            // 空・空白のみ
    TooLong,          // 最大長を超えている
    InvalidStart,     // 先頭が英数字ではない
    InvalidCharacter, // 使えない文字を含む
};

using NameResult = std::variant<std::string, NameError>;

inline constexpr std::size_t kMaxPlayerNameLength = 16;

namespace detail {

inline bool isAsciiAlnum(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

// 許可する文字: ASCII 英数字・スペース・アンダースコア・ハイフン
inline bool isAllowedNameChar(char c) {
    return isAsciiAlnum(c) || c == ' ' || c == '_' || c == '-';
}

} // namespace detail

// プレイヤー名を検証する純粋関数（許可リスト方式）。
// 先頭を英数字に限定することで、CSV を表計算ソフトで開いたときの
// 数式インジェクション（= + - @ で始まる値）も入力段階で防ぐ。
inline NameResult parsePlayerName(std::string_view text) {
    const std::string_view name = strutil::trim(text);

    if (name.empty()) {
        return NameError::Empty;
    }
    if (name.size() > kMaxPlayerNameLength) {
        return NameError::TooLong;
    }
    if (!detail::isAsciiAlnum(name.front())) {
        return NameError::InvalidStart;
    }
    for (const char c : name) {
        if (!detail::isAllowedNameChar(c)) {
            return NameError::InvalidCharacter;
        }
    }
    return std::string(name);
}
