#include "itemtable.h"


int ItemTable::rowAt(QPoint pos)
{
    pos = mapFromGlobal(pos);
    pos = {pos.x(), pos.y() - this->horizontalHeader()->height()};
    int row {this->indexAt(pos).row()};
    this->selectRow(row);
    return row;
}

void ItemTable::remove(int row)
{
    _items.erase(row);
    this->removeRow(row);
}
