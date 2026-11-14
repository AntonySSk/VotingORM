cmake_minimum_required(VERSION 3.21)
project(VotingORM LANGUAGES CXX)

add_compile_options(/utf-8)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Пошук підключених через vcpkg бібліотек
find_package(SqliteOrm CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Створення виконуваного файлу (надалі сюди додаватимуться інші файли реалізації)
add_executable(VotingORM
    src/main.cpp
)

# Підключення заголовкових файлів з папки include
target_include_directories(VotingORM
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Лінкування бібліотек ORM та SQLite
target_link_libraries(VotingORM
    PRIVATE
    sqlite_orm::sqlite_orm
    unofficial::sqlite3::sqlite3
)