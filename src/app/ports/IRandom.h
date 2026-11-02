#pragma once

// 乱数生成のポート（app が外部に求める機能）。実装は infra/RandomMT。
class IRandom {
public:
    virtual ~IRandom() = default;

    // [min, max] の範囲の整数を返す。min > max の場合は std::invalid_argument を投げる。
    virtual int generate(int min, int max) = 0;
};
