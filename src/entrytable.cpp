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
}

void EntryTable::add(std::unique_ptr<Item> item)
{
    std::unique_ptr<Entry> entry {dynamic_cast<Entry*>(item.release())};

    int row {-1};

    const QStringList decomposedCurrentDate {entry->date().split("/")};
    int d {decomposedCurrentDate.at(0).toInt()};
    int m {decomposedCurrentDate.at(1).toInt()};
    int y {decomposedCurrentDate.at(2).toInt()};
    const QDate currentDate {y, m, d};

    // careful: must update rows in map if insertion
    auto displaceItems {[this](int row) {
        for (int i = rowCount()-1; i>=row; --i) {
                auto node = _items.extract(i);
                node.key() = i+1;
                _items.insert(std::move(node));
            }}
    };

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
            displaceItems(row);
            this->insertRow(row);
            this->setSpan(i, 0, span+1, 1);
            break;
        } else if (date < currentDate) {
            row = i;
            displaceItems(row);
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

    _items.insert({row, std::move(entry)});
}

void EntryTable::setColumnsWidth()
{
    int tableWidth {this->width()};
    this->setColumnWidth(static_cast<int>(Col::DATE), 0.15*tableWidth);
    this->setColumnWidth(static_cast<int>(Col::FOOD), 0.2*tableWidth);
    this->setColumnWidth(static_cast<int>(Col::MASS), 0.099*tableWidth);
    this->setColumnWidth(static_cast<int>(Col::UNSATFATS), 0.15*tableWidth);
    this->setColumnWidth(static_cast<int>(Col::SATFATS), 0.15*tableWidth);
    this->setColumnWidth(static_cast<int>(Col::CARBS), 0.15*tableWidth);
    this->setColumnWidth(static_cast<int>(Col::PROTEINS), 0.10*tableWidth);
}

void EntryTable::updateFood(const Food &oldFood, const Food &newFood)
{
    for (int row = 0; row < rowCount(); ++row) {
        QLabel* labelName {dynamic_cast<QLabel*>(this->cellWidget(row, static_cast<int>(Col::FOOD)))};
        if (labelName && labelName->text() == oldFood.name()) {
            const auto& entry = static_cast<Entry*>(_items.at(row).get());
            if (!entry) continue;

            labelName->setText(newFood.name());
            double mass = newFood.density() == 0 ? entry->mass() : newFood.density() * entry->volume();

            dynamic_cast<QLabel*>(cellWidget(row, static_cast<int>(Col::MASS)))->setText(QString::number(mass));
            dynamic_cast<QLabel*>(cellWidget(row, static_cast<int>(Col::UNSATFATS)))->setText(QString::number(0.01 * std::roundf(100 * mass * newFood.unsaturatedFats()) / 100.f));
            dynamic_cast<QLabel*>(cellWidget(row, static_cast<int>(Col::SATFATS)))->setText(QString::number(0.01 * std::roundf(100 * mass * newFood.saturatedFats()) / 100.f));
            dynamic_cast<QLabel*>(cellWidget(row, static_cast<int>(Col::CARBS)))->setText(QString::number(0.01 * std::roundf(100 * mass * newFood.carbohydrates()) / 100.f));
            dynamic_cast<QLabel*>(cellWidget(row, static_cast<int>(Col::PROTEINS)))->setText(QString::number(0.01 * std::roundf(100 * mass * newFood.proteins()) / 100.f));
        }
    }
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

