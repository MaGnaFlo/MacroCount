#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <iostream>
#include <QMouseEvent>
#include "addfoodwidget.h"
#include "entrytable.h"
#include "addentrywidget.h"
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableEntries->setColumnsWidth();
    ui->tableFood->setColumnsWidth();

    ui->btnAddEntry->setEnabled(true);
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);

    ui->btnAddFood->setEnabled(true);
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);

    ui->btnAddEntry->setIcon(QIcon{QPixmap{"../../../images/cross"}});
    ui->btnAddEntry->setIconSize({ui->btnAddEntry->width()/2, ui->btnAddEntry->height()/2});
    ui->btnAddFood->setIcon(QIcon{QPixmap{"../../../images/apple_white"}});
    ui->btnAddFood->setIconSize({ui->btnAddFood->width()/2, ui->btnAddFood->height()/2});

    QMenu *menu {new QMenu("File")};
    QAction *openAction {new QAction("Open")};
    openAction->setShortcut(QKeySequence("CTRL+O"));
    QAction *saveAction {new QAction("Save")};
    saveAction->setShortcut(QKeySequence("CTRL+S"));
    QAction *saveAsAction {new QAction("Save as")};
    menu->addAction(openAction);
    menu->addAction(saveAction);
    menu->addAction(saveAsAction);
    QMenuBar *menuBar {new QMenuBar};
    menuBar->addMenu(menu);
    this->setMenuBar(menuBar);

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
    connect(ui->btnEditEntry, &QPushButton::clicked, this, &MainWindow::_editEntry);
    connect(ui->btnDeleteEntry, &QPushButton::clicked, this, &MainWindow::_deleteEntry);
    connect(ui->btnAddFood, &QPushButton::clicked, this, &MainWindow::_addFood);
    connect(ui->btnEditFood, &QPushButton::clicked, this, &MainWindow::_editFood);
    connect(ui->btnDeleteFood, &QPushButton::clicked, this, &MainWindow::_deleteFood);

    connect(openAction, &QAction::triggered, this, &MainWindow::_open);
    connect(saveAction, &QAction::triggered, this, &MainWindow::_save);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::_saveAs);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = mapToGlobal(event->pos());

    if (int row = ui->tableFood->rowAt(pos) != -1) {
        ui->btnDeleteFood->setEnabled(true);
        ui->btnEditFood->setEnabled(true);
    } else if (int row = ui->tableEntries->rowAt(pos) != -1) {
        ui->btnDeleteEntry->setEnabled(true);
        ui->btnEditEntry->setEnabled(true);
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::_addEntry()
{
    AddEntryWidget* entryWidget {new AddEntryWidget{ui->tableFood->items(), this}};
    entryWidget->setWindowTitle("Add new entry");
    if (entryWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Entry> entry {std::make_unique<Entry>(entryWidget->entry())};
        ui->tableEntries->add(std::move(entry));
    }
}

void MainWindow::_editEntry()
{
    const QItemSelectionModel* selectionModel {ui->tableEntries->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};
    const Entry entryAtRow {ui->tableEntries->entryFromRow(row)};

    AddEntryWidget* entryWidget {new AddEntryWidget {entryAtRow, ui->tableFood->items(), this}};
    entryWidget->setWindowTitle("Edit entry");
    if (entryWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Entry> entry {std::make_unique<Entry>(entryWidget->entry())};
        ui->tableEntries->fillRowFromEntry(row, *entry);
        ui->tableEntries->set(row, std::move(entry));
    }
}

void MainWindow::_deleteEntry()
{
    const QItemSelectionModel* selectionModel {ui->tableEntries->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};

    ui->tableEntries->remove(row);
    ui->tableEntries->clearSelection();
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);
}

void MainWindow::_addFood()
{
    AddFoodWidget* foodWidget {new AddFoodWidget};
    foodWidget->setWindowTitle("Add new food");
    if (foodWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Food> food {std::make_unique<Food>(foodWidget->food())};
        ui->tableFood->add(std::move(food));
    }
}

void MainWindow::_editFood()
{
    const QItemSelectionModel* selectionModel {ui->tableFood->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};
    const Food foodAtRow {ui->tableFood->foodFromRow(row)};

    AddFoodWidget* foodWidget {new AddFoodWidget {foodAtRow}};
    foodWidget->setWindowTitle("Edit food");
    if (foodWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Food> food {std::make_unique<Food>(foodWidget->food())};
        ui->tableEntries->updateFood(foodAtRow, *food);
        ui->tableFood->fillRowFromFood(row, *food);
        ui->tableFood->set(row, std::move(food));
    }
}

void MainWindow::_deleteFood()
{
    const QItemSelectionModel* selectionModel {ui->tableFood->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};

    int row {rowsModel.at(0).row()};
    ui->tableFood->remove(row);
    ui->tableFood->clearSelection();
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);
}

void MainWindow::_cbFoodChanged(int index) const
{
    if (index > 0) {
        ui->btnAddEntry->setEnabled(true);
    } else {
        ui->btnAddEntry->setEnabled(false);
    }
}

void MainWindow::_open()
{
    if (!_database.isOpen()) {
        const QString fileName {QFileDialog::getOpenFileName(this, QObject::tr("Open database"),
                                                            QDir::currentPath(),
                                                            QObject::tr("Database file (*.db)"))};
        if (fileName.isEmpty()) {
            return;
        }
        _database.setPath(fileName.toStdString());
    } else {
        _database.close(nullptr);
    }

    bool ok;
    _database.open(&ok);
    if (!ok) {
        QMessageBox msgBox;
        msgBox.setText("Error while opening the database!");
        msgBox.setInformativeText("Could not open the database.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }
}

void MainWindow::_save()
{
    if (!_database.isOpen()) {
        const QString fileName {QFileDialog::getOpenFileName(this, QObject::tr("Open database"),
                                                            QDir::currentPath(),
                                                            QObject::tr("Database file (*.db)"))};
        if (fileName.isEmpty()) {
            return;
        }
        _database.setPath(fileName.toStdString());
    }
}

void MainWindow::_saveAs()
{
    const QString fileName {QFileDialog::getOpenFileName(this, QObject::tr("Open database"),
                                                        QDir::currentPath(),
                                                        QObject::tr("Database file (*.db)"))};
    if (fileName.isEmpty()) {
        return;
    }

    ui->tableEntries->items();
}

