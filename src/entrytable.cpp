#include "entrytable.h"
#include <QHeaderView>
#include <QMouseEvent>
#include <iostream>
#include <QDate>


EntryTable::EntryTable(QWidget *parent) : ItemTable(parent)
{
    this->setRowCount(0);
    this->setColumnCount(7);
    this->setHorizontalHeaderLabels(
        {
            "Date",
            "Food",
            "Mass\n(g)",
            "Saturated fats\n(g)",
            "Unsaturated fats\n(g)",
            "Carbohydrates\n(g)",
            "Proteins\n(g)"
        });

    this->verticalHeader()->setVisible(false);

    setAttribute ( Qt::WA_TransparentForMouseEvents );
}

void EntryTable::add(std::unique_ptr<Item> item)
{
    std::unique_ptr<Entry> entry {static_cast<Entry*>(item.release())};

    int row {-1};

    const QStringList decomposedCurrentDate {entry->date().split("/")};
    int d {decomposedCurrentDate.at(0).toInt()};
    int m {decomposedCurrentDate.at(1).toInt()};
    int y {decomposedCurrentDate.at(2).toInt()};
    const QDate currentDate {y, m, d};

    for (int i = 0; i<rowCount(); ++i) {
        QLabel* label {dynamic_cast<QLabel*>(cellWidget(i, 0))};
        if (!label) {
            continue;
        }
        const QStringList decomposedDate {label->text().split("/")};
        int d {decomposedDate.at(0).toInt()};
        int m {decomposedDate.at(1).toInt()};
        int y {decomposedDate.at(2).toInt()};
        const QDate date {y, m, d};

        if (date == currentDate) {
            int span {rowSpan(i, 0)};
            row = i+span;
            this->insertRow(row);
            this->setSpan(i, 0, span+1, 1);
            break;
        } else if (date < currentDate) {
            row = i;
            this->insertRow(row);
            break;
        }
    }

    if (row == -1) {
        row = rowCount();
        this->insertRow(row);
    }

    setCellWidget(row, static_cast<int>(Col::DATE), new QLabel {entry->date()});
    setCellWidget(row, static_cast<int>(Col::FOOD), new QLabel {entry->food().name()});
    setCellWidget(row, static_cast<int>(Col::MASS), new QLabel {QString::number(entry->mass())});
    setCellWidget(row, static_cast<int>(Col::UNSATFATS), new QLabel {QString::number(entry->unsaturatedFats())});
    setCellWidget(row, static_cast<int>(Col::SATFATS), new QLabel {QString::number(entry->saturatedFats())});
    setCellWidget(row, static_cast<int>(Col::CARBS), new QLabel {QString::number(entry->carbohydrates())});
    setCellWidget(row, static_cast<int>(Col::PROTEINS), new QLabel {QString::number(entry->proteins())});

    _items.push_back(std::move(item));

}

void EntryTable::setColumnsWidth()
{
    int tableWidth {this->width()};
    std::cout << tableWidth << std::endl;
    this->setColumnWidth(0, 0.15*tableWidth);
    this->setColumnWidth(1, 0.2*tableWidth);
    this->setColumnWidth(2, 0.099*tableWidth);
    this->setColumnWidth(3, 0.15*tableWidth);
    this->setColumnWidth(4, 0.15*tableWidth);
    this->setColumnWidth(5, 0.15*tableWidth);
    this->setColumnWidth(6, 0.10*tableWidth);
}

Entry EntryTable::entryFromRow(int row) const
{
    Entry entry;
    for (int i = 0; i < columnCount(); ++i) {
        EntryTable::Col col {static_cast<EntryTable::Col>(i)};
        auto cellWidget {this->cellWidget(row, i)};
        if (!cellWidget) {
            continue;
        }
        QLabel* label {dynamic_cast<QLabel*>(cellWidget)};
        if (!label) {
            continue;
        }

        switch (col) {
        case EntryTable::Col::DATE:
            entry.setDate(label->text());
            break;
        case EntryTable::Col::FOOD:
            entry.setFood(Food(label->text()));
            break;
        case EntryTable::Col::MASS:
            entry.setMass(label->text().toDouble());
            break;
        case EntryTable::Col::UNSATFATS:
            entry.setUnsaturatedFats(label->text().toDouble());
            break;
        case EntryTable::Col::SATFATS:
            entry.setSaturatedFats(label->text().toDouble());
            break;
        case EntryTable::Col::CARBS:
            entry.setCarbohydrates(label->text().toDouble());
            break;
        case EntryTable::Col::PROTEINS:
            entry.setProteins(label->text().toDouble());
            break;
        }
    }
    return entry;
}

void EntryTable::fillRowFromEntry(int row, const Entry &entry)
{
    for (int i = 0; i < columnCount(); ++i) {
        EntryTable::Col col {static_cast<EntryTable::Col>(i)};
        auto cellWidget {this->cellWidget(row, i)};
        if (!cellWidget) {
            continue;
        }
        QLabel* label {dynamic_cast<QLabel*>(cellWidget)};
        if (!label) {
            continue;
        }

        switch (col) {
        case EntryTable::Col::DATE:
            label->setText(entry.date());
            break;
        case EntryTable::Col::FOOD:
            label->setText(entry.food().name());
            break;
        case EntryTable::Col::MASS:
            label->setText(QString::number(entry.mass()));
            break;
        case EntryTable::Col::UNSATFATS:
            label->setText(QString::number(entry.unsaturatedFats()));
            break;
        case EntryTable::Col::SATFATS:
            label->setText(QString::number(entry.saturatedFats()));
            break;
        case EntryTable::Col::CARBS:
            label->setText(QString::number(entry.carbohydrates()));
            break;
        case EntryTable::Col::PROTEINS:
            label->setText(QString::number(entry.proteins()));
            break;
        }
    }
}

