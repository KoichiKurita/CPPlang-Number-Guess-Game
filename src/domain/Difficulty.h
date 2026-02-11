#pragma once

#include <string>

struct Difficulty {
    int min;
    int max;
    std::string name;

    static Difficulty Easy() {
        return {1, 10, "Easy"};
    }

    static Difficulty Normal() {
        return {1, 50, "Normal"};
    }

    static Difficulty Hard() {
        return {1, 100, "Hard"};
    }
};
