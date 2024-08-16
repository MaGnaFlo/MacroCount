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
    // adjust map indices accordingly
    for (int i = row+1; i<rowCount(); ++i) {
        auto node = _items.extract(i);
        node.key() = i-1;
        _items.insert(std::move(node));
    }
    this->removeRow(row);
}

void ItemTable::clear()
{
    for (;;) {
        if (this->rowCount() == 0) {
            break;
        }
        this->remove(0);
    }
}
