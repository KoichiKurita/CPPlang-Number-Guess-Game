#pragma once
#include "IRandom.h"
#include <random>

class RandomMT : public IRandom {
public:
    int generate(int min, int max) override {
        static std::mt19937 gen{ std::random_device{}() };
        std::uniform_int_distribution<> dist(min, max);
        return dist(gen);
    }
};
