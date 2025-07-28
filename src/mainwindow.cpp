#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>
#include <QDate>
#include <iostream>
#include <QMouseEvent>
#include "addfoodwidget.h"
#include "entrytable.h"
#include "addentrywidget.h"
#include <QFileDialog>

#include <cpplotlib/figure.hpp>
#include <QtConcurrent/QtConcurrent>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("MacroCount");

    ui->tableEntries->setColumnsWidth();
    ui->tableFood->setColumnsWidth();

    ui->btnAddEntry->setEnabled(true);
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);

    ui->btnAddFood->setEnabled(true);
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);

    ui->btnAddEntry->setIcon(QIcon{QPixmap{"../../../images/cross"}});
    ui->btnAddEntry->setIconSize({ui->btnAddEntry->width()/2, ui->btnAddEntry->height()/2});
    ui->btnAddFood->setIcon(QIcon{QPixmap{"../../../images/apple_white"}});
    ui->btnAddFood->setIconSize({ui->btnAddFood->width()/2, ui->btnAddFood->height()/2});

    QMenu *menu {new QMenu("File")};
    QAction *newAction {new QAction("New")};
    newAction->setShortcut(QKeySequence("CTRL+N"));
    QAction *openAction {new QAction("Open")};
    openAction->setShortcut(QKeySequence("CTRL+O"));
    QAction *saveAction {new QAction("Save")};
    saveAction->setShortcut(QKeySequence("CTRL+S"));
    QAction *saveAsAction {new QAction("Save as")};
    menu->addAction(newAction);
    menu->addAction(openAction);
    menu->addAction(saveAction);
    menu->addAction(saveAsAction);
    QMenuBar *menuBar {new QMenuBar};
    menuBar->addMenu(menu);
    this->setMenuBar(menuBar);

    connect(ui->btnAddEntry, &QPushButton::clicked, this, &MainWindow::_addEntry);
    connect(ui->btnEditEntry, &QPushButton::clicked, this, &MainWindow::_editEntry);
    connect(ui->btnDeleteEntry, &QPushButton::clicked, this, &MainWindow::_deleteEntry);
    connect(ui->btnAddFood, &QPushButton::clicked, this, &MainWindow::_addFood);
    connect(ui->btnEditFood, &QPushButton::clicked, this, &MainWindow::_editFood);
    connect(ui->btnDeleteFood, &QPushButton::clicked, this, &MainWindow::_deleteFood);

    connect(newAction, &QAction::triggered, this, &MainWindow::_new);
    connect(openAction, &QAction::triggered, this, &MainWindow::_open);
    connect(saveAction, &QAction::triggered, this, &MainWindow::_save);
    connect(saveAsAction, &QAction::triggered, this, &MainWindow::_saveAs);

    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::_computeFigure);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    QPoint pos = mapToGlobal(event->pos());
    int rowFood {ui->tableFood->rowAt(pos)};
    int rowEntry {ui->tableEntries->rowAt(pos)};

    if (ui->mainTabWidget->currentIndex() == 1 && rowFood != -1) {
        ui->btnDeleteFood->setEnabled(true);
        ui->btnEditFood->setEnabled(true);
    } else if (ui->mainTabWidget->currentIndex() == 0 && rowEntry != -1) {
        ui->btnDeleteEntry->setEnabled(true);
        ui->btnEditEntry->setEnabled(true);
    }

    QMainWindow::mousePressEvent(event);
}

void MainWindow::_addEntry()
{
    AddEntryWidget* entryWidget {new AddEntryWidget{ui->tableFood->items(), this}};
    entryWidget->setWindowTitle("Add new entry");
    if (entryWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Entry> entry {std::make_unique<Entry>(entryWidget->entry())};
        ui->tableEntries->add(std::move(entry));
    }
}

void MainWindow::_editEntry()
{
    const QItemSelectionModel* selectionModel {ui->tableEntries->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};
    Entry entryAtRow {ui->tableEntries->entryFromRow(row)};

    AddEntryWidget* entryWidget {new AddEntryWidget {entryAtRow, ui->tableFood->items(), this}};
    entryWidget->setWindowTitle("Edit entry");
    if (entryWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Entry> entry {std::make_unique<Entry>(entryWidget->entry())};
        ui->tableEntries->fillRowFromEntry(row, *entry);
        ui->tableEntries->set(row, std::move(entry));
    }
}

void MainWindow::_deleteEntry()
{
    const QItemSelectionModel* selectionModel {ui->tableEntries->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};

    ui->tableEntries->remove(row);
    ui->tableEntries->clearSelection();
    ui->btnDeleteEntry->setEnabled(false);
    ui->btnEditEntry->setEnabled(false);
}

void MainWindow::_addFood()
{
    AddFoodWidget* foodWidget {new AddFoodWidget};
    foodWidget->setWindowTitle("Add new food");
    if (foodWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Food> food {std::make_unique<Food>(foodWidget->food())};
        ui->tableFood->add(std::move(food));
    }
}

void MainWindow::_editFood()
{
    const QItemSelectionModel* selectionModel {ui->tableFood->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};
    int row {rowsModel.at(0).row()};
    const Food foodAtRow {ui->tableFood->foodFromRow(row)};

    AddFoodWidget* foodWidget {new AddFoodWidget {foodAtRow}};
    foodWidget->setWindowTitle("Edit food");
    if (foodWidget->exec() == QDialog::Accepted) {
        std::unique_ptr<Food> food {std::make_unique<Food>(foodWidget->food())};
        ui->tableEntries->updateFood(foodAtRow, *food);
        ui->tableFood->fillRowFromFood(row, *food);
        ui->tableFood->set(row, std::move(food));
    }
}

void MainWindow::_deleteFood()
{
    const QItemSelectionModel* selectionModel {ui->tableFood->selectionModel()};
    const auto rowsModel {selectionModel->selectedRows()};

    int row {rowsModel.at(0).row()};
    ui->tableFood->remove(row);
    ui->tableFood->clearSelection();
    ui->btnDeleteFood->setEnabled(false);
    ui->btnEditFood->setEnabled(false);
}

void MainWindow::_cbFoodChanged(int index) const
{
    if (index > 0) {
        ui->btnAddEntry->setEnabled(true);
    } else {
        ui->btnAddEntry->setEnabled(false);
    }
}

void MainWindow::_new()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "Creating new table", "This will erase the current content. "
                                                             "Do you wish to proceed?",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        _database.close(nullptr);
        _database.setPath("");
        ui->tableEntries->clear();
        ui->tableFood->clear();
    }
}

void MainWindow::_computeFigure()
{
    double gSatFat {30}, gUnsatFat {20}, gCarbs {5}, gProteins {10};
    for (int i = 0; i<ui->tableEntries->rowCount(); ++i) {
        const Entry entry {ui->tableEntries->entryFromRow(i)};
        gSatFat += entry.saturatedFats();
        gUnsatFat += entry.unsaturatedFats();
        gCarbs += entry.carbohydrates();
        gProteins += entry.proteins();
    }

    plt::Figure figure{ui->label->width(), ui->label->height(), 100};

    if (!figure.init()) {
        std::cerr << "Could not initialize the Python interpreter." << std::endl;
        return;
    }

    std::vector<double> x_pie {{gSatFat, gUnsatFat, gCarbs, gProteins}};

    figure.addPlot(plt::PlotType::PIE, x_pie, {{"labels","['Saturated fats', 'Unsaturated fats', 'Carbs', 'Proteins']"}});


    if (!figure.build()) {
        std::cerr << "An Error occurred when building the figure." << std::endl;
    }

    figure.close();

    QImage image{figure.data(), figure.width(), figure.height(), figure.width() * 3, QImage::Format_RGB888};
    QPixmap pixmap {QPixmap::fromImage(image)};
    ui->label->setPixmap(pixmap);

}

void MainWindow::_open()
{
    QMessageBox::StandardButton reply;
    reply = QMessageBox::warning(this, "Opening database", "Opening another database will erase the current content. "
                                                           "Do you wish to proceed?",
                                 QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::No) {
        return;
    }

    if (!_database.isOpen()) {
        const QString fileName {QFileDialog::getOpenFileName(this, QObject::tr("Open database"),
                                                            QDir::currentPath(),
                                                            QObject::tr("Database file (*.db)"))};
        if (fileName.isEmpty()) {
            return;
        }
        _database.setPath(fileName.toStdString());
    } else {
        _database.close(nullptr);
    }

    bool ok;
    _database.open(&ok);
    if (!ok) {
        QMessageBox msgBox;
        msgBox.setText("Error while opening the database!");
        msgBox.setInformativeText("Could not open the database.");
        msgBox.setStandardButtons(QMessageBox::Ok);
        msgBox.setDefaultButton(QMessageBox::Ok);
        msgBox.exec();
    }

    // clear tables
    ui->tableEntries->clear();
    ui->tableFood->clear();

    // foods
    const sql::Selection foodSelection {_database.select("Foods", {"*"}, &ok)};
    if (!ok | !foodSelection.has_value()) return;

    for (const auto& row : foodSelection.value()) {
        std::unique_ptr<Food> food {std::make_unique<Food>()};
        food->setName(QString::fromStdString(std::get<std::string>(row.at("Name"))));
        food->setDensity(std::get<double>(row.at("Density")));
        food->setUnsaturatedFats(std::get<double>(row.at("Unsaturated_fats")));
        food->setSaturatedFats(std::get<double>(row.at("Saturated_fats")));
        food->setCarbohydrates(std::get<double>(row.at("Carbohydrates")));
        food->setProteins(std::get<double>(row.at("Proteins")));

        ui->tableFood->add(std::move(food));
    }

    // entries
    const sql::Selection entrySelection {_database.select("Entries", {"*"}, &ok)};
    if (!ok | !entrySelection.has_value()) return;

    for (const auto& row : entrySelection.value()) {
        std::unique_ptr<Entry> entry {std::make_unique<Entry>()};
        entry->setDate(QString::fromStdString(std::get<std::string>(row.at("Date"))));
        entry->setMass(std::get<double>(row.at("Mass")));
        entry->setVolume(std::get<double>(row.at("Volume")));
        entry->setUnsaturatedFats(std::get<double>(row.at("Unsaturated_fats")));
        entry->setSaturatedFats(std::get<double>(row.at("Saturated_fats")));
        entry->setCarbohydrates(std::get<double>(row.at("Carbohydrates")));
        entry->setProteins(std::get<double>(row.at("Proteins")));
        entry->setFood(Food{QString::fromStdString(std::get<std::string>(row.at("Food")))});

        ui->tableEntries->add(std::move(entry));
    }
}

void MainWindow::_writeDatabase()
{
    bool ok {true};
    if (!_database.isOpen()) {
        _database.open(&ok);
    }
    if (!ok) return;

    _database.drop("Foods", nullptr);
    _database.drop("Entries", nullptr);

    // foods
    _database.createTable("Foods", "ID INTEGER PRIMARY KEY, "
                                   "Name TEXT NOT NULL, "
                                   "Density REAL NOT NULL, "
                                   "Unsaturated_fats REAL NOT NULL, "
                                   "Saturated_fats REAL NOT NULL, "
                                   "Carbohydrates REAL NOT NULL,"
                                   "Proteins REAL NOT NULL",
                          &ok);
    if (!ok) return;

    for (int i = 0; i<ui->tableFood->rowCount(); ++i) {
        const Food food {ui->tableFood->foodFromRow(i)};
        std::stringstream data;
        data << i << ",";
        data << "'" << food.name().toStdString() << "'" << ",";
        data << food.density() << ",";
        data << food.unsaturatedFats() << ",";
        data << food.saturatedFats() << ",";
        data << food.carbohydrates() << ",";
        data << food.proteins();
        _database.insert("Foods", data.str(), nullptr);
    }

    // entries
    _database.createTable("Entries", "ID INTEGER PRIMARY KEY, "
                                     "Date TEXT NOT NULL, "
                                     "Food REAL NOT NULL, "
                                     "Mass REAL NOT NULL, "
                                     "Volume REAL NOT NULL, "
                                     "Unsaturated_fats REAL NOT NULL, "
                                     "Saturated_fats REAL NOT NULL, "
                                     "Carbohydrates REAL NOT NULL,"
                                     "Proteins REAL NOT NULL",
                          &ok);

    for (int i = 0; i<ui->tableEntries->rowCount(); ++i) {
        const Entry entry {ui->tableEntries->entryFromRow(i)};
        std::stringstream data;
        data << i << ",";
        data << "'" << entry.date().toStdString() << "'" << ",";
        data << "'" << entry.food().name().toStdString() << "'" << ",";
        data << entry.mass() << ",";
        data << entry.volume() << ",";
        data << entry.unsaturatedFats() << ",";
        data << entry.saturatedFats() << ",";
        data << entry.carbohydrates() << ",";
        data << entry.proteins();
        _database.insert("Entries", data.str(), nullptr);
    }
}

void MainWindow::_save()
{
    if (!_database.isOpen()) {
        const QString fileName {QFileDialog::getSaveFileName(this, QObject::tr("Save database"),
                                                            QDir::currentPath(),
                                                            QObject::tr("Database file (*.db)"))};
        if (fileName.isEmpty()) {
            return;
        }
        _database.setPath(fileName.toStdString());
    }
    _writeDatabase();
}

void MainWindow::_saveAs()
{
    const QString fileName {QFileDialog::getSaveFileName(this, QObject::tr("Save database"),
                                                        QDir::currentPath(),
                                                        QObject::tr("Database file (*.db)"))};
    if (fileName.isEmpty()) {
        return;
    }
     _database.setPath(fileName.toStdString());
    _writeDatabase();
}

