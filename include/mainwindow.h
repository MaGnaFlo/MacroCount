#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum class Col : int {
        DATE = 0,
        FOOD,
        MASS,
        SATFAT,
        UNSATFAT,
        CARBS,
        PROTEINS
    };

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    void _initializeEntriesTable();

private slots:
    void _addEntry();
    void _setEntryColumn(Col col, const QString& value);
    void _addFood();
};

#endif // MAINWINDOW_H
