#pragma once
#include <string>

class IScoreRepository {
public:
    virtual ~IScoreRepository() = default;
    virtual void save(const std::string& difficulty, int attempts) = 0;
};
