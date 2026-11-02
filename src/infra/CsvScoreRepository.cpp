#include "infra/CsvScoreRepository.h"

#include <fstream>

void CsvScoreRepository::save(const std::string& difficulty, int attempts) {
    std::ofstream file("scores.csv", std::ios::app);
    file << difficulty << "," << attempts << "\n";
}
