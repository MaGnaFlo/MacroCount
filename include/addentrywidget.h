#pragma once

#include <QObject>
#include <QWidget>
#include <QDialog>
#include "item.h"


QT_BEGIN_NAMESPACE
namespace Ui { class AddEntryWidget; }
QT_END_NAMESPACE

class AddEntryWidget : public QDialog
{
    Q_OBJECT

public:
    AddEntryWidget(std::map<int, std::unique_ptr<Item>> &foods, QWidget *parent = nullptr);
    AddEntryWidget(const Entry& entry, std::map<int, std::unique_ptr<Item>> &foods, QWidget *parent = nullptr);
    ~AddEntryWidget();
    const Entry& entry() const {return _entry;}

private slots:
    void validate();

private:
    Ui::AddEntryWidget *ui;
    Entry _entry;
    std::map<int, std::unique_ptr<Item>>& _foods;
};

