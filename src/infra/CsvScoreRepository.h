#pragma once
#include "IScoreRepository.h"
#include <fstream>

class CsvScoreRepository : public IScoreRepository {
public:
    void save(const std::string& diff, int attempts) override {
        std::ofstream file("scores.csv", std::ios::app);
        file << diff << "," << attempts << "\n";
    }
};
