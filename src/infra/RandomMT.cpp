#include "infra/RandomMT.h"

#include <stdexcept>

RandomMT::RandomMT() : engine_(std::random_device{}()) {}

RandomMT::RandomMT(std::mt19937::result_type seed) : engine_(seed) {}

int RandomMT::generate(int min, int max) {
    // min > max のまま uniform_int_distribution を作ると未定義動作になるため、先に弾く
    if (min > max) {
        throw std::invalid_argument("RandomMT::generate: min must be <= max");
    }
    std::uniform_int_distribution<int> dist(min, max);
    return dist(engine_);
}
