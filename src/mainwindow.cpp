#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <iostream>
#include <QMouseEvent>


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

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
    connect(ui->btnAddFood, &QPushButton::clicked, this, &MainWindow::_addFood);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = mapToGlobal(event->pos());
    int row {ui->tableFood->rowAt(pos)};

    QLabel* foodName {dynamic_cast<QLabel*>(ui->tableFood->cellWidget(row, static_cast<int>(FoodTable::Col::NAME)))};
    QLabel* foodDensity {dynamic_cast<QLabel*>(ui->tableFood->cellWidget(row, static_cast<int>(FoodTable::Col::DENSITY)))};
    QLabel* foodUnsatFats {dynamic_cast<QLabel*>(ui->tableFood->cellWidget(row, static_cast<int>(FoodTable::Col::UNSATFATS)))};
    QLabel* foodSatFats {dynamic_cast<QLabel*>(ui->tableFood->cellWidget(row, static_cast<int>(FoodTable::Col::SATFATS)))};
    QLabel* foodCarbs {dynamic_cast<QLabel*>(ui->tableFood->cellWidget(row, static_cast<int>(FoodTable::Col::CARBS)))};
    QLabel* foodProteins {dynamic_cast<QLabel*>(ui->tableFood->cellWidget(row, static_cast<int>(FoodTable::Col::PROTEINS)))};

    if (foodName) {
        ui->lineEditFoodName->setText(foodName->text());
    }
    if (foodDensity) {
        ui->spinDensity->setValue(foodDensity->text().toDouble());
    }
    if (foodUnsatFats) {
        ui->spinUnsatFatPerc->setValue(foodUnsatFats->text().toDouble());
    }
    if (foodSatFats) {
        ui->spinSatFatPerc->setValue(foodSatFats->text().toDouble());
    }
    if (foodCarbs) {
        ui->spinCarbsPerc->setValue(foodCarbs->text().toDouble());
    }
    if (foodProteins) {
        ui->spinProtPerc->setValue(foodProteins->text().toDouble());
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
    Food food {ui->lineEditFoodName->text()};
    auto foods {ui->tableFood->foods()};
    if (std::find_if(foods.begin(), foods.end(), [&food](const Food& food_)
                     {return food.name() == food_.name();}) != foods.end()) {
        QMessageBox msgBox;
        msgBox.setText("Food already exists");
        msgBox.setInformativeText("The food you're trying to add already exists in the list.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    QString densityText {ui->spinDensity->text()};
    densityText.replace(",", ".");
    food.setDensity(densityText.toDouble());
    food.setMacronutrients(ui->spinUnsatFatPerc->text().toDouble(),
                           ui->spinSatFatPerc->text().toDouble(),
                           ui->spinCarbsPerc->text().toDouble(),
                           ui->spinProtPerc->text().toDouble());
    ui->tableFood->addFood(food);
    ui->cbFood->addItem(food.name());
}

