#pragma once

#include "app/ports/IClock.h"

#include <cstdint>

// OS の時計（std::chrono::system_clock）による IClock の実装
class SystemClock : public IClock {
public:
    [[nodiscard]] std::int64_t nowUnixSeconds() const override;
};
