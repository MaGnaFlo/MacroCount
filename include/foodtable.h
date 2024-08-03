#pragma once

#include <QTableWidget>
#include <vector>
#include <QLabel>
#include <concepts>
#include "food.h"

namespace Ui { class FoodTable; }

class FoodTable : public QTableWidget
{
public:
    enum class Col {
        NAME = 0,
        DENSITY,
        UNSATFAT,
        SATFAT,
        CARBS,
        PROTEINS
    };
    explicit FoodTable(QWidget *parent = nullptr);
    std::vector<Food> foods() const {return _foods;}

public slots:
    void addFood(const Food& food);

private:
    void _setCell(int row, Col col, const QString& value);
    void _setCell(int row, Col col, double value);
    std::vector<Food> _foods;
};
