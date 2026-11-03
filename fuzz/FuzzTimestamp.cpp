// 日時の文字列の解析（timestamp::parse）のファジング用の関数。
// スコアファイルの日時の列は利用者が編集・改ざんできるため、どんな文字列でもクラッシュしないことに加えて、
// 次の性質を確認する。
//   1. 解析に成功したら、値は扱える範囲内で、文字列に戻すと元の文字列と完全に一致する
//      （同じ日時を表す書き方が 1 通りしかない ＝ 厳密に検証できている）
//   2. 範囲内の任意の値は、文字列にして解析し直すと元の値に戻る
#include "domain/Timestamp.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    // libFuzzer の入力はバイト列で渡されるので、文字列として読む
    const std::string_view text(reinterpret_cast<const char*>(data), size);

    // 性質 1
    if (const auto seconds = timestamp::parse(text)) {
        if (*seconds < timestamp::kMinSeconds || *seconds > timestamp::kMaxSeconds ||
            timestamp::format(*seconds) != text) {
            std::abort();
        }
    }

    // 性質 2（入力の先頭 8 バイトを、範囲内の値に変換して使う）
    if (size >= sizeof(std::uint64_t)) {
        std::uint64_t raw = 0;
        std::memcpy(&raw, data, sizeof(raw));
        const auto seconds =
            static_cast<std::int64_t>(raw % static_cast<std::uint64_t>(timestamp::kMaxSeconds + 1));
        const auto again = timestamp::parse(timestamp::format(seconds));
        if (!again || *again != seconds) {
            std::abort();
        }
    }
    return 0;
}
