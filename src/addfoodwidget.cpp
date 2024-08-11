#include "addfoodwidget.h"
#include "ui_addfoodwidget.h"
#include <QMessageBox>


AddFoodWidget::AddFoodWidget(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddFoodWidget)
{
    ui->setupUi(this);
    ui->checkBoxDensity->setChecked(true);
    ui->spinDensity->setEnabled(false);

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddFoodWidget::_validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->checkBoxDensity, &QCheckBox::clicked, this, &AddFoodWidget::_densityCheckBox);
}

AddFoodWidget::AddFoodWidget(const Food &food, QWidget *parent)
 : QDialog(parent), ui(new Ui::AddFoodWidget)
{
    ui->setupUi(this);

    ui->lineEditFoodName->setText(food.name());
    ui->spinUnsatFatPerc->setValue(food.unsaturatedFats());
    ui->spinSatFatPerc->setValue(food.unsaturatedFats());
    ui->spinCarbsPerc->setValue(food.carbohydrates());
    ui->spinProtPerc->setValue(food.proteins());

    if (food.density() == 0) {
        ui->checkBoxDensity->setChecked(true);
        ui->spinDensity->setEnabled(false);
    } else {
        ui->checkBoxDensity->setChecked(false);
        ui->spinDensity->setEnabled(true);
        ui->spinDensity->setValue(food.density());
    }

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddFoodWidget::_validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(ui->checkBoxDensity, &QCheckBox::clicked, this, &AddFoodWidget::_densityCheckBox);
}

AddFoodWidget::~AddFoodWidget()
{
    delete ui;
}

void AddFoodWidget::_validate()
{
    if (ui->lineEditFoodName->text().isEmpty()) {
        QMessageBox msgBox;
        msgBox.setText("Food name is empty!");
        msgBox.setInformativeText("Please enter a name for your food.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
        return;
    }

    _food = Food(ui->lineEditFoodName->text());
    QString densityText {ui->spinDensity->text()};
    densityText.replace(",", ".");
    _food.setDensity(densityText.toDouble());
    _food.setMacronutrients(ui->spinUnsatFatPerc->text().toDouble(),
                           ui->spinSatFatPerc->text().toDouble(),
                           ui->spinCarbsPerc->text().toDouble(),
                           ui->spinProtPerc->text().toDouble());
    this->accept();
}

void AddFoodWidget::_densityCheckBox(bool checked)
{
    if (checked) {
        ui->spinDensity->setValue(0);
        ui->spinDensity->setEnabled(false);
    } else {
        ui->spinDensity->setValue(1);
        ui->spinDensity->setEnabled(true);
    }
}
