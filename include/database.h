#pragma once

#include <sqlite3.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <iostream>
#include <sstream>

namespace sql {

    // clauses
    constexpr char const* WHERE {"WHERE"};
    constexpr char const* INNER_JOIN {"INNER JOIN"};
    constexpr char const* LEFT_JOIN {"LEFT JOIN"};
    constexpr char const* RIGHT_JOIN {"RIGHT JOIN"};
    constexpr char const* HAVING {"HAVING"};
    constexpr char const* ORDER {"ORDER BY"};
    constexpr char const* GROUP {"GROUP BY"};
    constexpr char const* OFFSET {"OFFSET"};
    constexpr char const* LIMIT {"LIMIT"};

    using Selection = std::optional<std::vector<std::unordered_map<std::string, std::variant<int, double, std::string, std::pair<void*,int>>>>>;

    /**
     * @brief Database class
     */
    class Database
    {
    public:
        Database() {}
        /**
         * @brief Constructor
         * @param path File path to database
         */
        Database(const std::string& path);

        /**
         * @brief Destructor
         */
        ~Database();

        // deleted copy and move
        Database(const Database&) = delete;
        Database(Database&&) = delete;
        const Database& operator=(const Database&) = delete;
        Database&& operator=(Database&&) = delete;

        /**
         * @brief Assess if the database is currently open
         * @return true is already open
         */
        bool isOpen() const {return _isOpen;}

        /**
         * @brief Opens the database
         * @param ok Pointer to boolean assessing correct execution
         */
        void open(bool *ok);

        /**
         * @brief Closes the database
         * @param ok Pointer to boolean assessing correct execution
         */
        void close(bool *ok);

        /**
         * @brief Creates a table in the database
         * @param table Table name
         * @param columnData Columns information (e.g. "ID INT PRIMARY KEY NOT NULL, ...")
         * @param ok Pointer to boolean assessing correct execution
         */
        void createTable(const std::string& table, const std::string& columnData, bool* ok);

        /**
         * @brief Drops a table from the database
         * @param table Table name
         * @param ok Pointer to boolean assessing correct execution
         */
        void drop(const std::string& table, bool* ok);

        /**
         * @brief Inserts a row in a specific table
         * @param table Table name
         * @param data Insertion query
         * @param ok
         */
        void insert(const std::string& table, const std::string& data, bool* ok);

        /**
         * @brief Removes a entry from the database, subject to a WHERE clause
         * @param table Table name
         * @param where WHERE clause
         * @param ok Pointer to boolean assessing correct execution
         */
        void remove(const std::string& table, const std::string& where, bool* ok);

        /**
         * @brief Selects data from a table of the database
         * @param table Table name
         * @param columns Columns to retrieve. If '*', all columns are retrieved
         * @param ok Pointer to boolean assessing correct execution
         * @param clauses Additional conditional clauses
         * @return An optional returned vector of retrieved rows (map<colname, val>)
         */
        template<class... T>
        Selection select(const std::string& table, const std::vector<std::string>& columns, bool *ok, T&&... clauses);

        /**
         * @brief Gets the value at i in the current statement
         * @param stmt SQL statement
         * @param i Retrieval index
         * @return Value at i
         */
        std::variant<int, double, std::string, std::pair<void*,int>> get(sqlite3_stmt* stmt, size_t i);

        /**
         * @brief Sets file path to database
         * @param path File path
         */
        void setPath(const std::string& path) {_path = path;}

    private:
        //! SQLite database
        sqlite3* _sqliteDB = nullptr;
        //! Database path
        std::string _path;
        bool _isOpen = false;

    };

    template<class... T>
    Selection Database::select(const std::string &table, const std::vector<std::string> &columns, bool *ok, T&&... clauses)
    {
        bool rc {true};

        // build query
        std::stringstream queryBuffer;
        queryBuffer << "SELECT ";
        bool first {true};
        for (const auto& col : columns) {
            if (!first) queryBuffer << ", ";
            else first = false;
            queryBuffer << col;
        }
        queryBuffer << " FROM " << table << " ";

        // additional clauses
        ([&] {queryBuffer << " " << clauses  << std::endl;} (), ...);

        const std::string& query {queryBuffer.str()};

        // number of columns
        size_t colCount {columns.size()};
        std::vector<std::string> columnNames;
        if (colCount == 1 && columns[0] == "*") {
            std::stringstream countQueryBuffer;
            countQueryBuffer <<  "SELECT * FROM " << table << " " << LIMIT << " 1;" << table << ";";
            sqlite3_stmt* stmt;
            int exit = sqlite3_prepare_v2(_sqliteDB, countQueryBuffer.str().c_str(), -1, &stmt, NULL);
            if (exit != SQLITE_OK) {
                std::cerr << "Failed to prepare COUNT query" << std::endl;
            } else {
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                    colCount = sqlite3_column_count(stmt);
                    for (int i = 0; i < colCount; i++) {
                        const char* colName = sqlite3_column_name(stmt, i);
                        columnNames.push_back(colName);
                    }
                }
            }
            sqlite3_finalize(stmt);
        }

        // retrieval
        std::vector<std::unordered_map<std::string, std::variant<int, double, std::string, std::pair<void*,int>>>> results;
        sqlite3_stmt* stmt;
        int exit = sqlite3_prepare_v2(_sqliteDB, query.c_str(), -1, &stmt, NULL);
        if (exit != SQLITE_OK) {
            std::cerr << "Failed to execute selection query" << std::endl;
            rc = false;
        } else {
            while(sqlite3_step(stmt) == SQLITE_ROW) {
                std::unordered_map<std::string, std::variant<int, double, std::string, std::pair<void*,int>>> colValues;
                for (size_t i = 0; i<colCount; ++i) {
                    colValues[columnNames[i]] = get(stmt, i);
                }
                results.push_back(colValues);
            }
            sqlite3_finalize(stmt);
        }

        // wrap up
        if (ok) {
            *ok = rc;
        }
        if (rc) {
            return results;
        } else {
            return {};
        }
    }

    /**
     * @brief Prints the selected rows and columns
     * @param Selection obtained from the select method
     */
    void printSelection(const Selection &selection);

}
