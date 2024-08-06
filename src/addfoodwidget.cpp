#include "addfoodwidget.h"
#include "ui_addfoodwidget.h"
#include <QMessageBox>


AddFoodWidget::AddFoodWidget(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddFoodWidget)
{
    ui->setupUi(this);
    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddFoodWidget::validate);
    connect(ui->btnCancel, &QPushButton::clicked, this, &QDialog::reject);
}

AddFoodWidget::AddFoodWidget(const Food &food, QWidget *parent)
 : QDialog(parent), ui(new Ui::AddFoodWidget)
{
    ui->setupUi(this);

    ui->lineEditFoodName->setText(food.name());
    ui->spinDensity->setValue(food.density());
    ui->spinUnsatFatPerc->setValue(food.unsaturatedFats());
    ui->spinSatFatPerc->setValue(food.unsaturatedFats());
    ui->spinCarbsPerc->setValue(food.carbohydrates());
    ui->spinProtPerc->setValue(food.proteins());

    connect(ui->btnAddEdit, &QPushButton::clicked, this, &AddFoodWidget::validate);
}

AddFoodWidget::~AddFoodWidget()
{
    delete ui;
}

void AddFoodWidget::validate()
{
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
