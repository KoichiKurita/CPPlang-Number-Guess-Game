// InputParser（予想値・難易度の入力の解析）のファジング用の関数。
// ランダムな文字列を、各難易度の範囲と int 全体の範囲で解析し、クラッシュ・未定義動作がないことに加えて、
// 次の性質を確認する。
//   1. 成功したら、値は必ず [min, max] の範囲内
//   2. 成功した値を文字列に戻して解析し直すと、同じ値になる
//   3. 難易度の解析が成功したら、3 つの難易度のどれかになる
#include "domain/Difficulty.h"
#include "domain/InputParser.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <string>
#include <string_view>
#include <variant>

namespace {

void checkRange(std::string_view text, int min, int max) {
    const ParseResult result = parseIntInRange(text, min, max);
    const int* value = std::get_if<int>(&result);
    if (value == nullptr) {
        return;
    }
    if (*value < min || *value > max) {
        std::abort(); // 性質 1 に違反
    }
    const ParseResult again = parseIntInRange(std::to_string(*value), min, max);
    const int* againValue = std::get_if<int>(&again);
    if (againValue == nullptr || *againValue != *value) {
        std::abort(); // 性質 2 に違反
    }
}

} // namespace

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    // libFuzzer の入力はバイト列で渡されるので、文字列として読む
    const std::string_view text(reinterpret_cast<const char*>(data), size);

    const std::array difficulties{Difficulty::Easy(), Difficulty::Normal(), Difficulty::Hard()};
    for (const Difficulty& difficulty : difficulties) {
        checkRange(text, difficulty.min, difficulty.max);
    }
    checkRange(text, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

    if (const auto difficulty = parseDifficulty(text)) {
        if (difficulty->name != "Easy" && difficulty->name != "Normal" &&
            difficulty->name != "Hard") {
            std::abort(); // 性質 3 に違反
        }
    }
    return 0;
}
