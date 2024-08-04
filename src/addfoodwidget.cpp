#include "addfoodwidget.h"
#include "ui_addfoodwidget.h"
#include <QMessageBox>


AddFoodWidget::AddFoodWidget(QWidget *parent)
    : QDialog(parent), ui(new Ui::AddFoodWidget)
{
    ui->setupUi(this);
    connect(ui->btnAdd, &QPushButton::clicked, this, &AddFoodWidget::add);
}

AddFoodWidget::~AddFoodWidget()
{
    delete ui;
}

void AddFoodWidget::add()
{
    _food = Food(ui->lineEditFoodName->text());
    QString densityText {ui->spinDensity->text()};
    densityText.replace(",", ".");
    _food.setDensity(densityText.toDouble());
    _food.setMacronutrients(ui->spinUnsatFatPerc->text().toDouble(),
                           ui->spinSatFatPerc->text().toDouble(),
                           ui->spinCarbsPerc->text().toDouble(),
                           ui->spinProtPerc->text().toDouble());
    this->close();
}
