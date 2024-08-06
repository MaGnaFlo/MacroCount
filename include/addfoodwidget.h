#ifndef ADDFOODWIDGET_H
#define ADDFOODWIDGET_H

#include <QObject>
#include <QWidget>
#include <QDialog>
#include "food.h"


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

private slots:
    void validate();

private:
    Ui::AddFoodWidget *ui;
    Food _food;

};

#endif // ADDFOODWIDGET_H
