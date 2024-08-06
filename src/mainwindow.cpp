#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <iostream>
#include <QMouseEvent>
#include "addfoodwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _initializeEntriesTable();

    for (const Food& food : ui->tableFood->foods()) {
        ui->cbFood->addItem(food.name());
    }

    QDate date {QDate::currentDate()};
    ui->editDate->setDate(date);

    ui->btnAddEntry->setEnabled(false);
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);

    ui->btnAddFood->setEnabled(true);
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
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
    int row {ui->tableFood->rowAt(pos)};

    if (row != -1) {
        ui->btnDeleteFood->setEnabled(true);
        ui->btnEditFood->setEnabled(true);
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::_initializeEntriesTable()
{
    ui->tableEntries->setRowCount(0);
    ui->tableEntries->setColumnCount(7);
    ui->tableEntries->setHorizontalHeaderLabels(
        {
            "Date",
            "Food",
            "Mass\n(g)",
            "Saturated fats\n(g)",
            "Unsaturated fats\n(g)",
            "Carbohydrates\n(g)",
            "Proteins\n(g)"
        });

    int tableWidth {ui->tableEntries->width()};
    ui->tableEntries->setColumnWidth(0, 0.15*tableWidth);
    ui->tableEntries->setColumnWidth(1, 0.2*tableWidth);
    ui->tableEntries->setColumnWidth(2, 0.099*tableWidth);
    ui->tableEntries->setColumnWidth(3, 0.15*tableWidth);
    ui->tableEntries->setColumnWidth(4, 0.15*tableWidth);
    ui->tableEntries->setColumnWidth(5, 0.15*tableWidth);
    ui->tableEntries->setColumnWidth(6, 0.10*tableWidth);
    ui->tableEntries->verticalHeader()->setVisible(false);
}

Food MainWindow::_rowToFood(int row) const
{
    Food food;
    for (int i = 0; i < ui->tableFood->columnCount(); ++i) {
        FoodTable::Col col {static_cast<FoodTable::Col>(i)};
        auto cellWidget {ui->tableFood->cellWidget(row, i)};
        if (!cellWidget) {
            continue;
        }
        QLabel* label {dynamic_cast<QLabel*>(cellWidget)};
        if (!label) {
            continue;
        }

        switch (col) {
        case FoodTable::Col::NAME:
            food.setName(label->text());
            break;
        case FoodTable::Col::UNSATFATS:
            food.setUnsaturatedFats(label->text().toDouble());
            break;
        case FoodTable::Col::SATFATS:
            food.setSaturatedFats(label->text().toDouble());
            break;
        case FoodTable::Col::CARBS:
            food.setCarbohydrates(label->text().toDouble());
            break;
        case FoodTable::Col::PROTEINS:
            food.setProteins(label->text().toDouble());
            break;
        case FoodTable::Col::DENSITY:
            food.setDensity(label->text().toDouble());
            break;
        }
    }
    return food;
}

void MainWindow::_foodToRow(const Food& food, int row) const
{
    for (int i = 0; i < ui->tableFood->columnCount(); ++i) {
        FoodTable::Col col {static_cast<FoodTable::Col>(i)};
        auto cellWidget {ui->tableFood->cellWidget(row, i)};
        if (!cellWidget) {
            continue;
        }
        QLabel* label {dynamic_cast<QLabel*>(cellWidget)};
        if (!label) {
            continue;
        }

        switch (col) {
        case FoodTable::Col::NAME:
            label->setText(food.name());
            break;
        case FoodTable::Col::UNSATFATS:
            label->setText(QString::number(food.unsaturatedFats()));
            break;
        case FoodTable::Col::SATFATS:
            label->setText(QString::number(food.saturatedFats()));
            break;
        case FoodTable::Col::CARBS:
            label->setText(QString::number(food.carbohydrates()));
            break;
        case FoodTable::Col::PROTEINS:
            label->setText(QString::number(food.proteins()));
            break;
        case FoodTable::Col::DENSITY:
            label->setText(QString::number(food.density()));
            break;
        }
    }
}

void MainWindow::_addEntry()
{
    const QString& foodName {ui->cbFood->currentText()};
    const auto foods {ui->tableFood->foods()};
    const auto food_it {std::find_if(foods.begin(), foods.cend(), [&foodName](const Food& food){return food.name() == foodName;})};

    if (foods.empty()) {
        QMessageBox msgBox;
        msgBox.setText("No food available!");
        msgBox.setInformativeText("No food selected. Add food items in the 'food' tab before adding entries in this table.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if (food_it == foods.cend()) {
        QMessageBox msgBox;
        msgBox.setText("Could not add entry");
        msgBox.setInformativeText("An unexpected error occurred.\nCould not add entry.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    const Food& food {*food_it};

    std::unordered_map<Col, QString> values {
        {Col::DATE, ui->editDate->text()},
        {Col::FOOD, ui->cbFood->currentText()},
        {Col::MASS, ui->spinMass->text()},
        {Col::SATFAT, QString::number(ui->spinMass->text().toDouble() * food.saturatedFats())},
        {Col::UNSATFAT, QString::number(ui->spinMass->text().toDouble() * food.unsaturatedFats())},
        {Col::CARBS, QString::number(ui->spinMass->text().toDouble() * food.carbohydrates())},
        {Col::PROTEINS, QString::number(ui->spinMass->text().toDouble() * food.proteins())}
    };

    const auto& value_empty_if {std::find_if(values.begin(), values.end(), [](const auto& item){return item.second.size()==0;})};
    if (value_empty_if != values.end()) {
        QMessageBox msgBox;
        msgBox.setText("Empty fields");
        msgBox.setInformativeText("Some fields in your entry are empty.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    int row {-1};

    const QStringList decomposedCurrentDate {ui->editDate->text().split("/")};
    int d {decomposedCurrentDate.at(0).toInt()};
    int m {decomposedCurrentDate.at(1).toInt()};
    int y {decomposedCurrentDate.at(2).toInt()};
    const QDate currentDate {y, m, d};

    for (int i = 0; i<ui->tableEntries->rowCount(); ++i) {
        QLabel* label {dynamic_cast<QLabel*>(ui->tableEntries->cellWidget(i, 0))};
        if (!label) {
            continue;
        }
        const QStringList decomposedDate {label->text().split("/")};
        int d {decomposedDate.at(0).toInt()};
        int m {decomposedDate.at(1).toInt()};
        int y {decomposedDate.at(2).toInt()};
        const QDate date {y, m, d};

        if (date == currentDate) {
            int span {ui->tableEntries->rowSpan(i, 0)};
            row = i+span;
            ui->tableEntries->insertRow(row);
            ui->tableEntries->setSpan(i, 0, span+1, 1);
            break;
        } else if (date < currentDate) {
            row = i;
            ui->tableEntries->insertRow(row);
            break;
        }
    }

    if (row == -1) {
        row = ui->tableEntries->rowCount();
        ui->tableEntries->insertRow(row);
    }

    for (const auto& [col, value] : values) {
        QLabel* text = new QLabel {value};
        ui->tableEntries->setCellWidget(row, static_cast<int>(col), text);
    }
}

void MainWindow::_addFood()
{
    AddFoodWidget* foodWidget {new AddFoodWidget};
    if (foodWidget->exec() == QDialog::Accepted) {
        const Food& food {foodWidget->food()};
        ui->tableFood->addFood(food);
        ui->cbFood->addItem(food.name());
    }
}

void MainWindow::_editFood()
{
    const QItemSelectionModel* selectionModel {ui->tableFood->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};
    const Food foodAtRow {_rowToFood(row)};

    AddFoodWidget* foodWidget {new AddFoodWidget {foodAtRow}};
    if (foodWidget->exec() == QDialog::Accepted) {
        const Food& food {foodWidget->food()};
        _foodToRow(food, row);
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

