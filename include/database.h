#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>


class Database
{
public:
    /**
     * @brief Constructor
     * @param path File path to database
     */
    Database(const std::string& path);

    /**
     * @brief Destructor
     */
    ~Database();

    /**
     * @brief Opens the database
     * @param ok Pointer to boolean assessing correct execution
     */
    void open(bool *ok = nullptr);

    /**
     * @brief Creates a table in the database
     * @param table Table name
     * @param columnData Columns information (e.g. "ID INT PRIMARY KEY NOT NULL, ...")
     * @param ok
     */
    void createTable(const std::string& table, const std::string& columnData, bool* ok = nullptr);

    /**
     * @brief Drops a table from the database
     * @param table Table name
     * @param ok
     */
    void drop(const std::string& table, bool* ok = nullptr);

    /**
     * @brief Inserts a row in a specific table
     * @param table Table name
     * @param data Insertion query
     * @param ok
     */
    void insert(const std::string& table, const std::string& data, bool* ok = nullptr);

    /**
     * @brief Removes a entry from the database, subject to a WHERE clause
     * @param table Table name
     * @param where WHERE clause
     * @param ok
     */
    void remove(const std::string& table, const std::string& where, bool* ok = nullptr);

    /**
     * @brief Selects data from a table of the database
     * @param table Table name
     * @param columns Columns to retrieve. If '*', all columns are retrieved
     * @param clauses Additional conditional clauses
     * @param ok
     * @return An optional returned vector of retrieved rows (map<colname, val>)
     */
    std::optional<std::vector<std::unordered_map<std::string, std::variant<int, double, std::string, std::pair<void*,int>>>>>
    select(const std::string& table, const std::vector<std::string>& columns, const std::string& clauses, bool *ok = nullptr);

    /**
     * @brief Gets the value at i in the current statement
     * @param stmt SQL statement
     * @param i Retrieval index
     * @return Value at i
     */
    std::variant<int, double, std::string, std::pair<void*,int>> get(sqlite3_stmt* stmt, size_t i);

private:
    //! SQLite database
    sqlite3* _sqliteDB = nullptr;
    //! Database path
    std::string _path;


};
