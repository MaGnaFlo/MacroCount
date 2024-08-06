#pragma once

#include <QString>

class Food
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
    void setMacronutrients(double unsaturatedFats, double saturatedFats, double carbohydrates, double proteins)
    {
        _unsaturatedFats = unsaturatedFats;
        _saturatedFats = saturatedFats;
        _carbohydrates = carbohydrates;
        _proteins = proteins;
    }

    const QString& name() const {return _name;}
    double density() const {return _density;}
    double saturatedFats() const {return _saturatedFats;}
    double unsaturatedFats() const {return _unsaturatedFats;}
    double carbohydrates() const {return _carbohydrates;}
    double proteins() const {return _proteins;}

    void setName(const QString& name) {_name = name;}
    void setDensity(double density) {_density = density;}
    void setUnsaturatedFats(double unsaturatedFats) {_unsaturatedFats = unsaturatedFats;}
    void setSaturatedFats(double saturatedFats) {_saturatedFats = saturatedFats;}
    void setCarbohydrates(double carbohydrates) {_carbohydrates = carbohydrates;}
    void setProteins(double proteins) {_proteins = proteins;}

private:
    QString _name;
    double _density;
    double _unsaturatedFats {0.0f};
    double _saturatedFats {0.0f};
    double _carbohydrates {0.0f};
    double _proteins {0.0f};
};
