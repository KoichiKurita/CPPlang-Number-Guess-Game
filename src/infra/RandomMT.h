#pragma once

#include "app/ports/IRandom.h"

#include <random>

// メルセンヌ・ツイスタによる IRandom の実装。
class RandomMT : public IRandom {
public:
    // 既定: std::random_device で初期化（毎回異なる答えになる）
    RandomMT();
    // シード指定: 同じシードなら同じ列になる（再現が必要なデバッグ・テスト用）
    explicit RandomMT(std::mt19937::result_type seed);

    int generate(int min, int max) override;

private:
    std::mt19937 engine_;
};
