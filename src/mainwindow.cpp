#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    _initializeTable();

    connect(ui->btnAdd, &QPushButton::clicked, this, &MainWindow::_addEntry);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_initializeTable()
{
    ui->tableEntries->setRowCount(0);
    ui->tableEntries->setColumnCount(7);
    ui->tableEntries->setHorizontalHeaderLabels(
        {
            "Date",
            "Food",
            "Mass\n(g)",
            "Saturated fats\n(%)",
            "Unsaturated fats\n(%)",
            "Carbohydrates\n(%)",
            "Proteins\n(%)"
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

    std::unordered_map<Col, QString> values {
        {Col::DATE, ui->editDate->text()},
        {Col::FOOD, ui->lineEditFood->text()},
        {Col::MASS, ui->spinMass->text()},
        {Col::SATFAT, ui->spinSatFatGrams->text()},
        {Col::UNSATFAT, ui->spinUnsatFatGrams->text()},
        {Col::CARBS, ui->spinCarbsGrams->text()},
        {Col::PROTEINS, ui->spinProtGrams->text()}
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
