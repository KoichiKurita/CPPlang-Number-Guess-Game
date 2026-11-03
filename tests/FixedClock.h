#pragma once
#include "app/ports/IClock.h"

#include <cstdint>

// 常に決まった時刻を返す時計。日時を記録する振る舞いを、毎回同じ結果で確かめるために使う。
class FixedClock : public IClock {
public:
    // 既定の時刻: 2026-11-03T14:35:07Z
    static constexpr std::int64_t kDefault = 1793716507;

    explicit FixedClock(std::int64_t now = kDefault) : now_(now) {}

    std::int64_t nowUnixSeconds() const override { return now_; }

private:
    std::int64_t now_;
};
