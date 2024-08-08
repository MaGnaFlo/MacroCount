#pragma once

#include <QTableWidget>
#include <QHeaderView>
#include <vector>
#include <QLabel>
#include <iostream>
#include "item.h"
#include "itemtable.h"


namespace Ui { class FoodTable; }

class FoodTable : public ItemTable
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
    virtual ~FoodTable() {}

    virtual void add(std::unique_ptr<Item> item);

    Food foodFromRow(int row) const;
    void fillRowFromFood(int row, const Food& food);

};
