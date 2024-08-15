#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenuBar>
#include "database.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    virtual void mousePressEvent(QMouseEvent* event) override;

private:
    void _initializeEntriesTable();
    Ui::MainWindow *ui;
    sql::Database _database;

private slots:
    void _addEntry();
    void _editEntry();
    void _deleteEntry();
    void _addFood();
    void _editFood();
    void _deleteFood();
    void _cbFoodChanged(int) const;
    void _save();
    void _saveAs();
    void _open();
};

#endif // MAINWINDOW_H
