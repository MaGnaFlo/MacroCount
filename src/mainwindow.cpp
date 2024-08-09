#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <iostream>
#include <QMouseEvent>
#include "addfoodwidget.h"
#include "entrytable.h"
#include "addentrywidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableEntries->setColumnsWidth();

    ui->btnAddEntry->setEnabled(true);
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);

    ui->btnAddFood->setEnabled(true);
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
    connect(ui->btnEditEntry, &QPushButton::clicked, this, &MainWindow::_editEntry);
    connect(ui->btnDeleteEntry, &QPushButton::clicked, this, &MainWindow::_deleteEntry);
    connect(ui->btnAddFood, &QPushButton::clicked, this, &MainWindow::_addFood);
    connect(ui->btnEditFood, &QPushButton::clicked, this, &MainWindow::_editFood);
    connect(ui->btnDeleteFood, &QPushButton::clicked, this, &MainWindow::_deleteFood);
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
    if (entryWidget->exec() == QDialog::Accepted) {
        const Entry& entry {entryWidget->entry()};
        ui->tableEntries->fillRowFromEntry(row, entry);
    }
}

void MainWindow::_deleteEntry()
{
    const QItemSelectionModel* selectionModel {ui->tableEntries->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};

    ui->tableEntries->removeRow(row);
    ui->tableEntries->clearSelection();
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);
}

void MainWindow::_addFood()
{
    AddFoodWidget* foodWidget {new AddFoodWidget};
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
    if (foodWidget->exec() == QDialog::Accepted) {
        const Food& food {foodWidget->food()};
        ui->tableFood->fillRowFromFood(row, food);
    }
}

void MainWindow::_deleteFood()
{
    const QItemSelectionModel* selectionModel {ui->tableFood->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};

    ui->tableFood->removeRow(row);
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

