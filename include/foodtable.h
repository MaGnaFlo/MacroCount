#pragma once

#include <QTableWidget>
#include <QHeaderView>
#include <vector>
#include <QLabel>
#include <iostream>
#include "food.h"


namespace Ui { class FoodTable; }

class FoodTable : public QTableWidget
{
    Q_OBJECT

public:
    enum class Col {
        NAME = 0,
        DENSITY,
        UNSATFATS,
        SATFATS,
        CARBS,
        PROTEINS
    };
    explicit FoodTable(QWidget *parent = nullptr);
    std::vector<Food> foods() const {return _foods;}

    int rowAt(QPoint pos);


public slots:
    void addFood(const Food& food);

private:
    void _setCell(int row, Col col, const QString& value);
    void _setCell(int row, Col col, double value);
    std::vector<Food> _foods;
};
