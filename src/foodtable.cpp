#include "foodtable.h"
#include <QHeaderView>

FoodTable::FoodTable(QWidget *parent) : QTableWidget(parent)
{
    this->setColumnCount(6);
    this->setRowCount(0);
    this->verticalHeader()->setVisible(false);
    this->setHorizontalHeaderLabels(
        {
            "Name",
            "Density",
            "Saturated fats\n(%)",
            "Unsaturated fats\n(%)",
            "Carbohydrates\n(%)",
            "Proteins\n(%)"
        });
}

void FoodTable::addFood(const Food &food)
{
    int row {rowCount()};
    this->insertRow(row);

    this->_setCell(row, Col::NAME, food.name());
    this->_setCell(row, Col::DENSITY, food.density());
    this->_setCell(row, Col::UNSATFAT, food.unsaturatedFats());
    this->_setCell(row, Col::SATFAT, food.saturatedFats());
    this->_setCell(row, Col::CARBS, food.carbohydrates());
    this->_setCell(row, Col::PROTEINS, food.proteins());

    _foods.push_back(food);
}

void FoodTable::_setCell(int row, Col col, const QString &value)
{
    QLabel *label {new QLabel{value}};
    setCellWidget(row, static_cast<int>(col), label);
}

void FoodTable::_setCell(int row, Col col, double value)
{
    QLabel *label {new QLabel{QString::number(value)}};
    setCellWidget(row, static_cast<int>(col), label);
}
