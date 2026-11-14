#pragma once
#include "ICandidateRepository.h"
#include "Candidate.h"
#include <sqlite_orm/sqlite_orm.h>

template <typename Storage>
class CandidateRepositoryImpl : public ICandidateRepository {
private:
    Storage& storage;

public:
    explicit CandidateRepositoryImpl(Storage& s) : storage(s) {}

    int add(const Candidate& candidate) override {
        return storage.insert(candidate);
    }

    std::optional<Candidate> getById(int id) override {
        return storage.template get_optional<Candidate>(id);
    }

    std::vector<Candidate> getAll() override {
        return storage.template get_all<Candidate>();
    }

    void update(const Candidate& candidate) override {
        storage.update(candidate);
    }

    void remove(int id) override {
        storage.template remove<Candidate>(id);
    }
};