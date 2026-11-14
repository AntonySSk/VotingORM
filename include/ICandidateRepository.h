#pragma once
#include <vector>
#include <optional>
#include "Candidate.h"

class ICandidateRepository {
public:
    virtual ~ICandidateRepository() = default;

    virtual int add(const Candidate& candidate) = 0;
    virtual std::optional<Candidate> getById(int id) = 0;
    virtual std::vector<Candidate> getAll() = 0;
    virtual void update(const Candidate& candidate) = 0;
    virtual void remove(int id) = 0;
};