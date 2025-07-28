#pragma once

#include <QObject>
#include <QWidget>
#include <QDialog>
#include "item.h"


QT_BEGIN_NAMESPACE
namespace Ui { class AddFoodWidget; }
QT_END_NAMESPACE

class AddFoodWidget : public QDialog
{
    Q_OBJECT

public:
    AddFoodWidget(QWidget *parent = nullptr);
    AddFoodWidget(const Food& food, QWidget *parent = nullptr);
    ~AddFoodWidget();
    const Food& food() const {return _food;}

private Q_SLOTS:
    void _validate();
    void _densityCheckBox(bool);

private:
    Ui::AddFoodWidget *ui;
    Food _food;

};
