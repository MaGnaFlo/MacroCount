#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _initializeEntriesTable();

    for (const Food& food : ui->tableFood->foods()) {
        ui->cbFood->addItem(food.name());
    }

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
    connect(ui->btnAddFood, &QPushButton::clicked, this, &MainWindow::_addFood);
}

MainWindow::~MainWindow()
{
    delete ui;
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

    ui->tableEntries->insertRow(0);
    for (const auto& [col, value] : values) _setEntryColumn(col, value);
}

void MainWindow::_setEntryColumn(Col col, const QString &value)
{
    int i {static_cast<int>(col)};
    QLabel* text = new QLabel {value};
    ui->tableEntries->setCellWidget(0, i, text);
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

    food.setDensity(ui->spinDensity->text().toDouble());
    food.setMacronutrients(ui->spinUnsatFatPerc->text().toDouble(),
                           ui->spinSatFatPerc->text().toDouble(),
                           ui->spinCarbsPerc->text().toDouble(),
                           ui->spinProtPerc->text().toDouble());
    ui->tableFood->addFood(food);
    ui->cbFood->addItem(food.name());
}

