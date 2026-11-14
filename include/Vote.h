#pragma once
#include <string>

struct Vote {
    int id = 0;
    int candidateId = 0; // Зовнішній ключ на Candidate::id
    std::string votingStation;
    int voterAge = 0;
    long long timestamp = 0;
};