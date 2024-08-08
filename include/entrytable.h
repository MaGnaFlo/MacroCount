#pragma once

#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <iostream>
#include "item.h"
#include "itemtable.h"


namespace Ui { class EntryTable; }

class EntryTable : public ItemTable
{
    Q_OBJECT

public:
    enum class Col : int {
        DATE = 0,
        FOOD,
        MASS,
        SATFATS,
        UNSATFATS,
        CARBS,
        PROTEINS
    };

    explicit EntryTable(QWidget *parent = nullptr);
    virtual ~EntryTable() {}

    virtual void add(std::unique_ptr<Item> item);
    virtual void setColumnsWidth();
};
