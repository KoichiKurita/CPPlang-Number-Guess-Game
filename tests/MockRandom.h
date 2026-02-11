#pragma once
#include "../src/infra/IRandom.h"

class MockRandom : public IRandom {
public:
    explicit MockRandom(int fixed) : value_(fixed) {}
    int generate(int, int) override { return value_; }
private:
    int value_;
};
