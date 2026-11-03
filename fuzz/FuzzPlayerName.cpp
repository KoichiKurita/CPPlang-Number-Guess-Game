// プレイヤー名の検証（parsePlayerName）のファジング用の関数。
// 検証を通った名前が、次の性質を必ず満たすことを確認する。
//   1. 長さは 1〜kMaxPlayerNameLength 文字で、先頭は英数字、使う文字は許可したものだけ
//   2. もう一度検証しても同じ名前になる（検証が安定している）
//   3. CSV に書き出すときに、数式インジェクションの対策（先頭の '）が必要にならない
//      （＝ 入力の段階で、表計算ソフトが数式と解釈する名前を受け付けていない）
#include "domain/PlayerName.h"
#include "infra/Csv.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string>
#include <string_view>
#include <variant>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    // libFuzzer の入力はバイト列で渡されるので、文字列として読む
    const std::string_view text(reinterpret_cast<const char*>(data), size);

    const NameResult result = parsePlayerName(text);
    const auto* name = std::get_if<std::string>(&result);
    if (name == nullptr) {
        return 0;
    }

    // 性質 1
    if (name->empty() || name->size() > kMaxPlayerNameLength ||
        !detail::isAsciiAlnum(name->front())) {
        std::abort();
    }
    for (const char c : *name) {
        if (!detail::isAllowedNameChar(c)) {
            std::abort();
        }
    }

    // 性質 2
    const NameResult again = parsePlayerName(*name);
    const auto* againName = std::get_if<std::string>(&again);
    if (againName == nullptr || *againName != *name) {
        std::abort();
    }

    // 性質 3
    if (csv::escapeField(*name) != *name) {
        std::abort();
    }
    return 0;
}
