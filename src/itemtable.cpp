#include "itemtable.h"


ItemTable::ItemTable(QWidget *parent) : QTableWidget(parent)
{
    this->verticalHeader()->setVisible(false);
    setAttribute(Qt::WA_TransparentForMouseEvents);
}

int ItemTable::rowAt(QPoint pos)
{
    pos = mapFromGlobal(pos);
    pos = {pos.x(), pos.y() - this->horizontalHeader()->height()};
    int row {this->indexAt(pos).row()};
    this->selectRow(row);
    return row;
}

void ItemTable::set(int row, std::unique_ptr<Item> item)
{
    _items[row] = std::move(item);
}

void ItemTable::remove(int row)
{
    _items.erase(row);
    this->removeRow(row);
}
