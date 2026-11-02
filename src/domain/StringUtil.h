#pragma once

#include <string_view>

namespace strutil {

// 前後の空白（スペース・タブ・改行）を取り除く
inline std::string_view trim(std::string_view s) {
    constexpr std::string_view ws = " \t\r\n";
    const auto first = s.find_first_not_of(ws);
    if (first == std::string_view::npos) {
        return {};
    }
    const auto last = s.find_last_not_of(ws);
    return s.substr(first, last - first + 1);
}

} // namespace strutil
