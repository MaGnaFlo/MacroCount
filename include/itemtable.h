#pragma once

#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <iostream>
#include "item.h"


namespace Ui { class ItemTable; }

class ItemTable : public QTableWidget
{
    Q_OBJECT

public:
    explicit ItemTable(QWidget *parent = nullptr) : QTableWidget(parent) {}
    virtual int rowAt(QPoint pos)
    {
        pos = mapFromGlobal(pos);
        pos = {pos.x(), pos.y() - this->horizontalHeader()->height()};
        int row {this->indexAt(pos).row()};
        this->selectRow(row);
        return row;
    }
    virtual void add(std::unique_ptr<Item> item) = 0;
    virtual void setColumnsWidth() {}
    const std::vector<std::unique_ptr<Item>>& items() const {return _items;}

protected:
    std::vector<std::unique_ptr<Item>> _items;
};
