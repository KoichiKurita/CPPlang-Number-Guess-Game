#pragma once
class IRandom {
public:
    virtual ~IRandom() = default;
    virtual int generate(int min, int max) = 0;
};
