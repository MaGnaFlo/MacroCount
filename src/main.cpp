#include "mainwindow.h"

#include <QApplication>
#include <sqlite3.h>
#include <iostream>

#include "database.h"

int main(int argc, char *argv[])
{

    // test
    sql::Database db("../../../mydb.db");
    db.open(nullptr);
    db.drop("TestTable", nullptr);

    db.createTable("TestTable",
                       "ID INTEGER PRIMARY KEY, "
                       "Name TEXT NOT NULL, "
                       "Age INTEGER ",
                   nullptr);

    db.insert("TestTable", "1, 'John', 31", nullptr);
    db.insert("TestTable", "2, 'Seth', 13", nullptr);
    db.insert("TestTable", "3, 'Tania', 79", nullptr);
    db.insert("TestTable", "4, 'Beth', 53", nullptr);
    db.insert("TestTable", "5, 'Samantha', 23", nullptr);

    sql::Selection selection = db.select("TestTable", {"*"}, nullptr,
                                sql::WHERE, "Age < 60",
                                sql::ORDER, "Age",
                                sql::LIMIT, "3");
    sql::printSelection(selection);
    // -

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
