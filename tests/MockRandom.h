#pragma once
#include "app/ports/IRandom.h"

// 常に固定値を返す乱数。受け取った範囲を記録して、呼び出し側の検証に使う。
class MockRandom : public IRandom {
public:
    explicit MockRandom(int fixed) : value_(fixed) {}

    int generate(int min, int max) override {
        lastMin_ = min;
        lastMax_ = max;
        ++callCount_;
        return value_;
    }

    int lastMin() const { return lastMin_; }
    int lastMax() const { return lastMax_; }
    int callCount() const { return callCount_; }

private:
    int value_;
    int lastMin_ = 0;
    int lastMax_ = 0;
    int callCount_ = 0;
};
