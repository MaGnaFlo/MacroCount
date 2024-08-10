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
    explicit ItemTable(QWidget *parent = nullptr);
    virtual ~ItemTable() {}

    int rowAt(QPoint pos);
    virtual void setColumnsWidth() = 0;

    virtual void add(std::unique_ptr<Item> item) = 0;
    void set(int row, std::unique_ptr<Item> item);
    void remove(int row);

    std::map<int, std::unique_ptr<Item>>& items() {return _items;}

protected:
    std::map<int, std::unique_ptr<Item>> _items;
};
