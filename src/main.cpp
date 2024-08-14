#include "mainwindow.h"

#include <QApplication>
#include <sqlite3.h>
#include <iostream>

#include "database.h"

int main(int argc, char *argv[])
{

    Database db("../../../mydb.db");
    db.open();
    db.drop("TestTable");
//    db.createTable("FOOD", "ID INT PRIMARY KEY NOT NULL, "
//                           "NAME TEXT NOT NULL,"
//                           "TYPE TEXT NOT NULL");

//    db.insert("FOOD", "(ID, NAME, TYPE) VALUES (1, Apple, \"Pépin\")");
//    db.insert("FOOD", "(ID, NAME, TYPE) VALUES (2, \"Orange\", \"Agrume\")");
//    db.insert("FOOD", "(ID, NAME, TYPE) VALUES (3, \"Citron\", \"Agrume\")");
//    db.insert("FOOD", "(ID, NAME, TYPE) VALUES (4, \"Pèche\", \"Drupe\")");
//    db.insert("FOOD", "(ID, NAME, TYPE) VALUES (5, \"Abricot\", \"Drupe\")");
//    db.insert("FOOD", "(ID, NAME, TYPE) VALUES (6, \"Tomate\", \"Baie\")");

    db.createTable("TestTable", "ID INTEGER PRIMARY KEY,"
                       "Name TEXT NOT NULL, "
                       "Age INTEGER, "
                       "Salary REAL, "
                       "ProfilePicture BLOB,"
                       "Notes TEXT DEFAULT NULL,"
                       "CreatedAt TEXT DEFAULT (datetime('now')),"
                       "Active INTEGER DEFAULT 1,"
                       "Misc BLOB");

    db.insert("TestTable", "(ID, Name, Age, Salary, ProfilePicture, Notes, CreatedAt, Active, Misc)"
                           "VALUES (1, 'John Doe', 30, 75000.50, X'89504E470D0A1A0A', 'Employee of the month', datetime('now'), 1, NULL)");

    auto results {db.select("TestTable", {"*"}, {""})};

    if (results.has_value()) {
        for (const auto& row : results.value()) {
            for (const auto& result : row) {
                const std::string& col = result.first;
                if (std::holds_alternative<int>(result.second)) {
                    auto value = std::get<int>(result.second);
                    std::cout << "int " << " " << result.first << ":" << value << " | ";
                } else if (std::holds_alternative<std::pair<void*, int>>(result.second)) {
                    auto value = std::get<std::pair<void*,int>>(result.second);
                    std::cout << "blob " << " " << result.first << ":" << value.second << " | ";
                } else if (std::holds_alternative<double>(result.second)) {
                    auto value = std::get<double>(result.second);
                    std::cout << "double " << " " << result.first << ":" << value << " | ";
                } else if (std::holds_alternative<std::string>(result.second)) {
                    auto value = std::get<std::string>(result.second);
                    std::cout << "text " << " " << result.first << ":" << value << " | ";
                }
            }
            std::cout << std::endl;
        }
    }

    //////////////////
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
