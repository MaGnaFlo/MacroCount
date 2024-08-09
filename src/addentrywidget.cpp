#include "addentrywidget.h"
#include "ui_addentrywidget.h"
#include <QMessageBox>


AddEntryWidget::AddEntryWidget(std::vector<std::unique_ptr<Item>> &foods, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddEntryWidget), _foods(foods)
{
    ui->setupUi(this);
    ui->editDate->setDate(QDate::currentDate());
    ui->cbFood->addItem("Select food");
    for (const auto& item : foods) {
        auto food {dynamic_cast<Food*>(item.get())};
        if (!food) continue;
        ui->cbFood->addItem(food->name());
    }

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddEntryWidget::validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddEntryWidget::reject);
}

AddEntryWidget::AddEntryWidget(const Entry &entry, std::vector<std::unique_ptr<Item> > &foods, QWidget *parent)
    : QDialog(parent), ui(new Ui::AddEntryWidget), _foods(foods), _entry(entry)
{
    ui->setupUi(this);
    ui->editDate->setDate(QDate::currentDate());
    ui->spinMass->setValue(entry.mass());
    ui->cbFood->addItem("Select food");
    for (const auto& item : foods) {
        auto food {dynamic_cast<Food*>(item.get())};
        if (!food) continue;
        ui->cbFood->addItem(food->name());
        ui->cbFood->setCurrentText(entry.food().name()); // what if you deleted the food?
    }

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddEntryWidget::validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &AddEntryWidget::reject);
}
AddEntryWidget::~AddEntryWidget()
{
    delete ui;
}

void AddEntryWidget::validate()
{
    const QString& foodName {ui->cbFood->currentText()};
    auto itemIt {std::find_if(_foods.cbegin(), _foods.cend(), [&foodName](const auto& foodItem)
                             {
                                 const auto& food {dynamic_cast<Food*>(foodItem.get())};
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

    auto foodPtr = dynamic_cast<Food*>(itemIt->get());
    const Food foodCpy {*foodPtr}; // stack copy
    _entry = Entry(foodCpy, ui->editDate->text(), ui->spinMass->text().toDouble());

    this->accept();
}
