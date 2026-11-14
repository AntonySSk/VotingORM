#include <iostream>
#include <string>
#include <cstdio>
#include <sqlite_orm/sqlite_orm.h>
#include "Candidate.h"
#include "Vote.h"
#include "ICandidateRepository.h"
#include "CandidateRepository.h"

inline auto createStorage(const std::string& filename) {
    return sqlite_orm::make_storage(filename,
        sqlite_orm::make_table("candidates",
            sqlite_orm::make_column("id", &Candidate::id, sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("full_name", &Candidate::fullName),
            sqlite_orm::make_column("party_name", &Candidate::partyName),
            sqlite_orm::make_column("age", &Candidate::age),
            sqlite_orm::make_column("rating_score", &Candidate::ratingScore)
        ),
        sqlite_orm::make_table("votes",
            sqlite_orm::make_column("id", &Vote::id, sqlite_orm::primary_key().autoincrement()),
            sqlite_orm::make_column("candidate_id", &Vote::candidateId),
            sqlite_orm::make_column("voting_station", &Vote::votingStation),
            sqlite_orm::make_column("voter_age", &Vote::voterAge),
            sqlite_orm::make_column("timestamp", &Vote::timestamp),
            sqlite_orm::foreign_key(&Vote::candidateId).references(&Candidate::id)
        )
    );
}

int main() {
    system("chcp 65001 > nul");

    std::string dbFile = "voting_system.sqlite";
    std::remove(dbFile.c_str());

    auto storage = createStorage(dbFile);
    storage.sync_schema();

    std::cout << "=== РЕАЛІЗАЦІЯ ТРАНЗАКЦІЙ ТА АТОМАРНОСТІ ===\n\n";

    // Початковий стан БД
    std::cout << "[СТАН 0] Кандидатів у БД: " << storage.get_all<Candidate>().size()
        << ", Голосів у БД: " << storage.get_all<Vote>().size() << "\n\n";

    // СЦЕНАРІЙ 1: Успішне виконання транзакції
    std::cout << "=== СЦЕНАРІЙ 1: Успішна транзакція ===\n";
    try {
        storage.transaction([&storage]() {
            // Додаємо кандидата
            int candId = storage.insert(Candidate{ 0, "Іван Петренко", "Партія Майбутнього", 42, 15.5 });
            // Додаємо голос для цього дійсного кандидата
            storage.insert(Vote{ 0, candId, "Дільниця №101", 25, 1731600000 });

            std::cout << "  > Транзакція виконана успішно, коміт підтверджено.\n";
            return true; // true означає COMMIT
            });
    }
    catch (const std::exception& e) {
        std::cout << "  [ПОМИЛКА] Виключення: " << e.what() << "\n";
    }

    std::cout << "[СТАН 1] Кандидатів у БД після успіху: " << storage.get_all<Candidate>().size()
        << ", Голосів у БД: " << storage.get_all<Vote>().size() << "\n\n";


    // СЦЕНАРІЙ 2: Помилка та відкат (Rollback)=
    std::cout << "=== СЦЕНАРІЙ 2: Транзакція з помилкою (порушення обмеження) ===\n";
    size_t candidatesBeforeError = storage.get_all<Candidate>().size();

    try {
        storage.transaction([&storage]() {
            // 1. Успішна операція всередині транзакції (додавання нового кандидата)
            int candId = storage.insert(Candidate{ 0, "Олена Коваленко", "Свобода Слова", 38, 24.8 });
            std::cout << "  > Додано тимчасового кандидата з ID: " << candId << "\n";

            // 2. Навмисна помилка: намагаємося додати голос для неіснуючого кандидата (ID 9999)
            // Це порушує обмеження зовнішнього ключа (foreign_key constraint)
            std::cout << "  > Спроба додати голос для неіснуючого кандидата (ID 9999)...\n";
            storage.insert(Vote{ 0, 9999, "Дільниця №999", 30, 1731600800 });

            return true;
            });
    }
    catch (const std::exception& e) {
        std::cout << "  [ПІВЕРДЖЕНО] Спіймано виключення бази даних: " << e.what() << "\n";
        std::cout << "  > Транзакція автоматично скасована (ROLLBACK).\n";
    }

    // Перевірка стану бази даних після помилки
    size_t candidatesAfterError = storage.get_all<Candidate>().size();
    std::cout << "[СТАН 2] Кандидатів у БД після відкату: " << candidatesAfterError
        << ", Голосів у БД: " << storage.get_all<Vote>().size() << "\n";

    if (candidatesBeforeError == candidatesAfterError) {
        std::cout << "  [УСПІХ] Атомарність дотримана: всі зміни попередньої операції транзакції скасовано!\n";
    }
    else {
        std::cout << "  [ПОМИЛКА] Зміни не були повністю відкочені.\n";
    }

    return 0;
}