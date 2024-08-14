#include "database.h"
#include <sstream>
#include <iostream>


Database::Database(const std::string &path)
    : _path{path}
{

}

Database::~Database()
{
    int rc = sqlite3_close(_sqliteDB);
    if (rc == SQLITE_BUSY) {
        std::cerr << "Unable to close the database because there are unfinalized prepared statements." << std::endl;
    } else if (rc != SQLITE_OK) {
        std::cerr << "Failed to close the database connection." << std::endl;
    } else {
        _sqliteDB = nullptr;
        std::cout << "Database connection closed successfully." << std::endl;
    }
}

void Database::open(bool *ok)
{
    bool rc = true;
    int exit {sqlite3_open(_path.c_str(), &_sqliteDB)};
    if (exit != SQLITE_OK) {
        std::cerr << "Error opening database " << _path << std::endl;
        rc = false;
    }
    if (ok) {
        *ok = rc;
    }
}

void Database::createTable(const std::string &table, const std::string &columnData, bool *ok)
{
    bool rc = true;
    std::stringstream queryBuffer;
    queryBuffer << "CREATE TABLE IF NOT EXISTS " << table << "(" << columnData << ");";
    const std::string& query {queryBuffer.str()};

    char* errorMsg;
    int exit = sqlite3_exec(_sqliteDB, query.c_str(), NULL, 0, &errorMsg);
    if (exit != SQLITE_OK) {
        std::cerr << "Error creating table (" << errorMsg << ")" << std::endl;
        sqlite3_free(errorMsg);
        rc = false;
    }
    if (ok) {
        *ok = rc;
    }
}

void Database::drop(const std::string &table, bool *ok)
{
    bool rc = true;
    std::stringstream queryBuffer;
    queryBuffer << "DROP TABLE IF EXISTS " << table << ";";
    const std::string& query {queryBuffer.str()};

    char* errorMsg;
    int exit = sqlite3_exec(_sqliteDB, query.c_str(), NULL, 0, &errorMsg);
    if (exit != SQLITE_OK) {
        std::cerr << "Error dropping table from database (" << errorMsg << ")" << std::endl;
        rc = false;
    }
    sqlite3_free(errorMsg);

    if (ok) {
        *ok = rc;
    }
}

void Database::insert(const std::string &table, const std::string &data, bool *ok)
{
    bool rc = true;
    std::stringstream queryBuffer;
    queryBuffer << "INSERT INTO " << table << " " << data << ";";

    const std::string query {queryBuffer.str()};

    char* errorMsg;
    int exit = sqlite3_exec(_sqliteDB, query.c_str(), NULL, 0, &errorMsg);
    if (exit != SQLITE_OK) {
        std::cerr << "Error during insertion (" << errorMsg << ")" << std::endl;
        rc = false;
    }
    sqlite3_free(errorMsg);

    if (ok) {
        *ok = rc;
    }
}

void Database::remove(const std::string &table, const std::string &where, bool *ok)
{
    bool rc = true;
    std::stringstream queryBuffer;
    queryBuffer << "DELETE FROM " << table << " ";
    if (!where.empty()) {
        queryBuffer << "WHERE " << where << ";";
    }
    const std::string& query {queryBuffer.str()};

    char* errorMsg;
    int exit = sqlite3_exec(_sqliteDB, query.c_str(), NULL, 0, &errorMsg);
    if (exit != SQLITE_OK) {
        std::cerr << "Error during deletion (" << errorMsg << ")" << std::endl;
        rc = false;
    }
    sqlite3_free(errorMsg);

    if (ok) {
        *ok = rc;
    }
}

std::optional<std::vector<std::unordered_map<std::string, std::variant<int, double, std::string, std::pair<void*,int>>>>>
Database::select(const std::string &table, const std::vector<std::string> &columns, const std::string &clauses, bool *ok)
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
    queryBuffer << " FROM " << table << " " << clauses << ";";
    const std::string& query {queryBuffer.str()};

    // number of columns
    size_t colCount {columns.size()};
    std::vector<std::string> columnNames;
    if (colCount == 1 && columns[0] == "*") {
        std::stringstream countQueryBuffer;
        countQueryBuffer <<  "SELECT * FROM " << table << " LIMIT 1;" << table << ";";
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

std::variant<int, double, std::string, std::pair<void*,int>>
Database::get(sqlite3_stmt *stmt, size_t i)
{
    std::variant<int, double, std::string, std::pair<void*,int>> value;
    int type {sqlite3_column_type(stmt, i)};
    switch (type) {
    case SQLITE_INTEGER: {
        value = sqlite3_column_int(stmt, i);
        break;
    }
    case SQLITE_FLOAT: {
        value = sqlite3_column_double(stmt, i);
        break;
    }
    case SQLITE_TEXT: {
        std::stringstream buffer;
        buffer << sqlite3_column_text(stmt, i);
        value = buffer.str();
        break;
    }
    case SQLITE_BLOB: {
        void* blobData {std::remove_const_t<void*>(sqlite3_column_blob(stmt, i))};
        int blobSize {sqlite3_column_bytes(stmt, i)};
        value = std::make_pair(blobData, blobSize);
        break;
    }
    case SQLITE_NULL: {
        value = "";
        break;
    }
    default: {
        value = "";
    }
    }
    return value;
}

