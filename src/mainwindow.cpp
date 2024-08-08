#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <iostream>
#include <QMouseEvent>
#include "addfoodwidget.h"
#include "entrytable.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tableEntries->setColumnsWidth();

    for (const std::unique_ptr<Item>& item : ui->tableFood->items()) {
        const auto food {dynamic_cast<Food*>(item.get())};
        ui->cbFood->addItem(food->name());
    }

    QDate date {QDate::currentDate()};
    ui->editDate->setDate(date);

    ui->btnAddEntry->setEnabled(false);
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);

    ui->btnAddFood->setEnabled(true);
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);

    ui->cbFood->addItem("Select food");

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
    connect(ui->btnAddFood, &QPushButton::clicked, this, &MainWindow::_addFood);
    connect(ui->btnEditFood, &QPushButton::clicked, this, &MainWindow::_editFood);
    connect(ui->btnDeleteFood, &QPushButton::clicked, this, &MainWindow::_deleteFood);
    connect(ui->cbFood, &QComboBox::currentIndexChanged, this, &MainWindow::_cbFoodChanged);
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
    const QString& foodName {ui->cbFood->currentText()};
    const auto& foodItems {ui->tableFood->items()};
    auto itemIt {std::find_if(foodItems.begin(), foodItems.end(), [&foodName](const auto& foodItem)
        {
            const auto& food {dynamic_cast<Food*>(foodItem.get())};
            return food->name() == foodName;
        }
    )};

    if (foodItems.empty()) {
        QMessageBox msgBox;
        msgBox.setText("No food available!");
        msgBox.setInformativeText("No food selected. Add food items in the 'food' tab before adding entries in this table.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if (itemIt == foodItems.cend()) {
        QMessageBox msgBox;
        msgBox.setText("Could not add entry");
        msgBox.setInformativeText("An unexpected error occurred.\nCould not add entry.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    auto foodPtr = dynamic_cast<Food*>(itemIt->get());
    const Food foodCpy {*foodPtr}; // stack copy
    std::unique_ptr<Entry> entry {std::make_unique<Entry>(foodCpy, ui->editDate->text(), ui->spinMass->text().toDouble())};
    ui->tableEntries->add(std::move(entry));
}

void MainWindow::_addFood()
{
    AddFoodWidget* foodWidget {new AddFoodWidget};
    if (foodWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Food> food {std::make_unique<Food>(foodWidget->food())};
        ui->cbFood->addItem(food->name());
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

