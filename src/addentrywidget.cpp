#include "addentrywidget.h"
#include "ui_addentrywidget.h"
#include <QMessageBox>


AddEntryWidget::AddEntryWidget(std::map<int, std::unique_ptr<Item> > &foods, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddEntryWidget), _foods(foods)
{
    ui->setupUi(this);
    ui->editDate->setDate(QDate::currentDate());
    ui->cbFood->addItem("Select food");
    for (const auto& item : foods) {
        auto food {dynamic_cast<Food*>(item.second.get())};
        if (!food) continue;
        ui->cbFood->addItem(food->name());
    }

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddEntryWidget::_validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddEntryWidget::reject);
}

AddEntryWidget::AddEntryWidget(Entry &entry, std::map<int, std::unique_ptr<Item> > &foods, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddEntryWidget), _foods(foods), _entry(entry)
{
    ui->setupUi(this);
    ui->editDate->setDate(QDate::fromString(entry.date()));

    const QString& foodName = entry.food().name();
    auto itemIt {std::find_if(_foods.cbegin(), _foods.cend(), [&foodName](const auto& foodItem)
                             {
                                 const auto& food {dynamic_cast<Food*>(foodItem.second.get())};
                                 return food->name() == foodName;
                             }
    )};

    if (itemIt != _foods.cend()) {
        Food* food = dynamic_cast<Food*>((*itemIt).second.get());
        entry.setFood(*food);
    }

    ui->editDate->setDate(QDate::fromString(entry.date(), "dd/MM/yyyy"));

    if (entry.food().density() == 0) {
        ui->spinMass->setValue(entry.mass());
    } else {
        ui->spinVolume->setValue(entry.volume());
    }
    ui->cbFood->addItem("Select food");
    for (const auto& [_, item] : foods) {
        auto food {dynamic_cast<Food*>(item.get())};
        if (!food) continue;
        ui->cbFood->addItem(food->name());
        ui->cbFood->setCurrentText(entry.food().name()); // what if you deleted the food?
    }

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddEntryWidget::_validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddEntryWidget::reject);
}
AddEntryWidget::~AddEntryWidget()
{
    delete ui;
}

void AddEntryWidget::_validate()
{
    const QString& foodName {ui->cbFood->currentText()};
    auto itemIt {std::find_if(_foods.cbegin(), _foods.cend(), [&foodName](const auto& foodItem)
                             {
                                 const auto& food {dynamic_cast<Food*>(foodItem.second.get())};
                                 return food->name() == foodName;
                             }
    )};

    if (ui->cbFood->count() == 1) {
        QMessageBox msgBox;
        msgBox.setText("No food available!");
        msgBox.setInformativeText("No food selected. Add food items in the 'food' tab before adding entries in this table.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    if (itemIt == _foods.cend()) {
        QMessageBox msgBox;
        msgBox.setText("Could not add entry");
        msgBox.setInformativeText("An unexpected error occurred.\nCould not add entry.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    auto foodPtr = dynamic_cast<Food*>((*itemIt).second.get());
    const Food foodCpy {*foodPtr}; // stack copy
    double mass = foodCpy.density() == 0 ? ui->spinMass->text().toDouble() : foodCpy.density() * ui->spinVolume->text().toDouble();
    _entry = Entry(foodCpy, ui->editDate->text(), mass, ui->spinVolume->text().toDouble());

    this->accept();
}
