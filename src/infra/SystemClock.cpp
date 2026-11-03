#include "infra/SystemClock.h"

#include <chrono>

std::int64_t SystemClock::nowUnixSeconds() const {
    // system_clock の起点は、C++20 で UNIX 時間の起点（1970-01-01T00:00:00Z）と規定された。
    // C++17 では未規定だが、対象の環境（Windows・Linux・macOS）の標準ライブラリはすべてこの起点を使う
    const auto sinceEpoch = std::chrono::system_clock::now().time_since_epoch();
    return static_cast<std::int64_t>(
        std::chrono::duration_cast<std::chrono::seconds>(sinceEpoch).count());
}
