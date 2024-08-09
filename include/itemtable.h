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
    int rowAt(QPoint pos);
    virtual void add(std::unique_ptr<Item> item) = 0;
    void remove(int row);
    virtual void setColumnsWidth() {}
    std::map<int, std::unique_ptr<Item>>& items() {return _items;}

protected:
    std::map<int, std::unique_ptr<Item>> _items;
};
