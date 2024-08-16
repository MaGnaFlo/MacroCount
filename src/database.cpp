#include "database.h"
#include <sstream>
#include <iostream>

namespace sql {

    Database::Database(const std::string &path)
        : _path{path}
    {

    }

    Database::~Database()
    {
        close(nullptr);
    }

    void Database::open(bool *ok)
    {
        bool rc = true;
        if (_path.empty()) {
            rc = false;
        } else {
            int exit {sqlite3_open(_path.c_str(), &_sqliteDB)};
            if (exit != SQLITE_OK) {
                std::cerr << "Error opening database " << _path << std::endl;
                rc = false;
            }
        }
        if (rc) {
            _isOpen = true;
        }
        if (ok) {
            *ok = rc;
        }
    }

    void Database::close(bool *ok)
    {
        int rc = sqlite3_close(_sqliteDB);
        if (rc == SQLITE_BUSY) {
            std::cerr << "Unable to close the database because there are unfinalized prepared statements." << std::endl;
        } else if (rc != SQLITE_OK) {
            std::cerr << "Failed to close the database connection." << std::endl;
        } else {
            _sqliteDB = nullptr;
        }

        if (rc == SQLITE_OK) {
            _isOpen = false;
        }
        if (ok) {
            *ok = rc == SQLITE_OK ? true : false;
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
            std::cerr << "Error creating table (" << (errorMsg ? errorMsg : "") << ")" << std::endl;
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
            std::cerr << "Error dropping table from database (" << (errorMsg ? errorMsg : "") << ")" << std::endl;
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
        queryBuffer << "INSERT INTO " << table << " VALUES (" << data << ");";
        std::cout << queryBuffer.str() << std::endl;

        const std::string query {queryBuffer.str()};

        char* errorMsg;
        int exit = sqlite3_exec(_sqliteDB, query.c_str(), NULL, 0, &errorMsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error during insertion (" << (errorMsg ? errorMsg : "") << ")" << std::endl;
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
            queryBuffer << WHERE << " " << where << ";";
        }
        const std::string& query {queryBuffer.str()};

        char* errorMsg;
        int exit = sqlite3_exec(_sqliteDB, query.c_str(), NULL, 0, &errorMsg);
        if (exit != SQLITE_OK) {
            std::cerr << "Error during deletion (" << (errorMsg ? errorMsg : "") << ")" << std::endl;
            rc = false;
        }
        sqlite3_free(errorMsg);

        if (ok) {
            *ok = rc;
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

    void printSelection(const sql::Selection &selection)
    {
        if (selection.has_value()) {
            for (const auto& row : selection.value()) {
                for (const auto& result : row) {
                    const std::string& col = result.first;
                    if (std::holds_alternative<int>(result.second)) {
                        auto value = std::get<int>(result.second);
                        std::cout << "int:" << " {" << result.first << ", " << value << "} | ";
                    } else if (std::holds_alternative<std::pair<void*, int>>(result.second)) {
                        auto value = std::get<std::pair<void*,int>>(result.second);
                        std::cout << "blob:" << " {" << result.first << ", size = " << value.second << "} | ";
                    } else if (std::holds_alternative<double>(result.second)) {
                        auto value = std::get<double>(result.second);
                        std::cout << "double:" << " {" << result.first << ", " << value << "} | ";
                    } else if (std::holds_alternative<std::string>(result.second)) {
                        auto value = std::get<std::string>(result.second);
                        std::cout << "text:" << " {" << result.first << ", " << value << "} | ";
                    } else {
                        std::cout << "NULL:" << " {" << result.first << ", NULL} |";
                    }
                }
                std::cout << std::endl;
            }
        }
    }

}
