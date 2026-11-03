#pragma once

#include <cstdint>

// 現在時刻のポート（app が外部に求める機能）。実装は infra/SystemClock。
// テストでは決まった時刻を返す実装を注入し、日時に依存する振る舞いを再現できるようにする。
class IClock {
public:
    virtual ~IClock() = default;

    // 現在時刻を UNIX 時間（1970-01-01T00:00:00Z からの秒数）で返す
    [[nodiscard]] virtual std::int64_t nowUnixSeconds() const = 0;
};
