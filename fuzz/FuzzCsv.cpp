// CSV の解析（csv::parseLine）のファジング用の関数。
// スコアファイルは利用者が編集・改ざんできるため、どんな行を読んでもクラッシュしないことに加えて、
// 次の性質（往復しても変わらないこと）を確認する。
//   解析できた行のフィールドを書き出し（formatLine）、もう一度解析すると、元と同じフィールドになる
#include "infra/Csv.h"

#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <string_view>

extern "C" int LLVMFuzzerTestOneInput(const std::uint8_t* data, std::size_t size) {
    // libFuzzer の入力はバイト列で渡されるので、文字列として読む
    const std::string_view line(reinterpret_cast<const char*>(data), size);

    const auto fields = csv::parseLine(line);
    if (!fields) {
        return 0; // 壊れた行は読み飛ばす仕様なので、解析の失敗は問題ではない
    }

    const auto again = csv::parseLine(csv::formatLine(*fields));
    if (!again || *again != *fields) {
        std::abort(); // 書き出して読み直すと、値が変わってしまう
    }
    return 0;
}
