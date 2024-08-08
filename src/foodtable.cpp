#include "foodtable.h"
#include <QHeaderView>
#include <QMouseEvent>
#include <iostream>


FoodTable::FoodTable(QWidget *parent) : ItemTable(parent)
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
    setAttribute ( Qt::WA_TransparentForMouseEvents );
}

void FoodTable::add(std::unique_ptr<Item> item)
{
    std::unique_ptr<Food> food {dynamic_cast<Food*>(item.release())};

    int row {rowCount()};
    this->insertRow(row);

    setCellWidget(row, static_cast<int>(Col::NAME), new QLabel {food->name()});
    setCellWidget(row, static_cast<int>(Col::DENSITY), new QLabel {QString::number(food->density())});
    setCellWidget(row, static_cast<int>(Col::UNSATFATS), new QLabel {QString::number(food->unsaturatedFats())});
    setCellWidget(row, static_cast<int>(Col::SATFATS), new QLabel {QString::number(food->saturatedFats())});
    setCellWidget(row, static_cast<int>(Col::CARBS), new QLabel {QString::number(food->carbohydrates())});
    setCellWidget(row, static_cast<int>(Col::PROTEINS), new QLabel {QString::number(food->proteins())});

    _items.push_back(std::move(food));
}

Food FoodTable::foodFromRow(int row) const
{
    Food food;
    for (int i = 0; i < columnCount(); ++i) {
        FoodTable::Col col {static_cast<FoodTable::Col>(i)};
        auto cellWidget {this->cellWidget(row, i)};
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

void FoodTable::fillRowFromFood(int row, const Food &food)
{
    for (int i = 0; i < columnCount(); ++i) {
        FoodTable::Col col {static_cast<FoodTable::Col>(i)};
        auto cellWidget {this->cellWidget(row, i)};
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
