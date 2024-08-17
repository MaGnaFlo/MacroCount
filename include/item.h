#pragma once

#include <QString>

/**
 * @brief General abstract item class for the tables
 */
class Item
{
public:
    Item() {}
    virtual ~Item() {}

    virtual double saturatedFats() const {return _saturatedFats;}
    virtual double unsaturatedFats() const {return _unsaturatedFats;}
    virtual double carbohydrates() const {return _carbohydrates;}
    virtual double proteins() const {return _proteins;}

    virtual void setUnsaturatedFats(double unsaturatedFats) {_unsaturatedFats = unsaturatedFats;}
    virtual void setSaturatedFats(double saturatedFats) {_saturatedFats = saturatedFats;}
    virtual void setCarbohydrates(double carbohydrates) {_carbohydrates = carbohydrates;}
    virtual void setProteins(double proteins) {_proteins = proteins;}

protected:
    double _unsaturatedFats {0.0f};
    double _saturatedFats {0.0f};
    double _carbohydrates {0.0f};
    double _proteins {0.0f};
};

/**
 * @brief Food item class for the food table
 */
class Food : public Item
{
public:
    Food() {}
    explicit Food(const QString& name) : _name(name) {}
    Food(const Food& other)
    {
        _name = other._name;
        _density = other._density;
        _unsaturatedFats = other._unsaturatedFats;
        _saturatedFats = other._saturatedFats;
        _carbohydrates = other._carbohydrates;
        _proteins = other._proteins;
    }

    const Food& operator=(const Food& other)
    {
        _name = other._name;
        _density = other._density;
        _unsaturatedFats = other._unsaturatedFats;
        _saturatedFats = other._saturatedFats;
        _carbohydrates = other._carbohydrates;
        _proteins = other._proteins;
        return *this;
    }

    Food(Food&& other)
    {
        _name = other._name;
        _density = other._density;
        _unsaturatedFats = other._unsaturatedFats;
        _saturatedFats = other._saturatedFats;
        _carbohydrates = other._carbohydrates;
        _proteins = other._proteins;
    }

    virtual ~Food() {}

    void setMacronutrients(double unsaturatedFats, double saturatedFats, double carbohydrates, double proteins)
    {
        setUnsaturatedFats(unsaturatedFats);
        setSaturatedFats(saturatedFats);
        setCarbohydrates(carbohydrates);
        setProteins(proteins);
    }

    const QString& name() const {return _name;}
    double density() const {return _density;}
    void setName(const QString& name) {_name = name;}
    void setDensity(double density) {_density = density;}

private:
    QString _name;
    double _density {0};

};

/**
 * @brief Entry item for the entry table
 */
class Entry : public Item
{
public:
    Entry() {}
    Entry(const Food& food, const QString& date, double mass, double volume = 0.f)
        : _food(food), _date(date), _mass(mass), _volume(volume)
    {
        setSaturatedFats(0.01 * std::roundf(100 * mass * food.saturatedFats()) / 100.f);
        setUnsaturatedFats(0.01 * std::roundf(100 * mass * food.unsaturatedFats()) / 100.f);
        setCarbohydrates(0.01 * std::roundf(100 * mass * food.carbohydrates()) / 100.f);
        setProteins(0.01 * std::roundf(100 * mass * food.proteins()) / 100.f);
    }

    Entry(const Entry& other)
    {
        _food = other._food;
        _date = other._date;
        _mass = other._mass;
        _volume = other._volume;
        _unsaturatedFats = other._unsaturatedFats;
        _saturatedFats = other._saturatedFats;
        _carbohydrates = other._carbohydrates;
        _proteins = other._proteins;
    }

    Entry(Entry&& other)
    {
        _food = other._food;
        _date = other._date;
        _mass = other._mass;
        _volume = other._volume;
        _unsaturatedFats = other._unsaturatedFats;
        _saturatedFats = other._saturatedFats;
        _carbohydrates = other._carbohydrates;
        _proteins = other._proteins;
    }

    Entry& operator=(Entry&& other)
    {
        _food = other._food;
        _date = other._date;
        _mass = other._mass;
        _volume = other._volume;
        _unsaturatedFats = other._unsaturatedFats;
        _saturatedFats = other._saturatedFats;
        _carbohydrates = other._carbohydrates;
        _proteins = other._proteins;
        return *this;
    }

    virtual ~Entry() {}

    const QString& date() const {return _date;}
    const Food& food() const {return _food;}
    double mass() const {return _mass;}
    double volume() const {return _volume;}
    double saturatedFats() const {return _saturatedFats;}
    double unsaturatedFats() const {return _unsaturatedFats;}
    double carbohydrates() const {return _carbohydrates;}
    double proteins() const {return _proteins;}

    void setDate(const QString& date) {_date = date;}
    void setMass(double mass) {_mass = mass;}
    void setVolume(double volume) {_volume = volume;}
    void setFood(const Food& food) {_food = food;}

private:
    Food _food;
    QString _date;
    double _mass {0.f};
    double _volume {0.f};

};
