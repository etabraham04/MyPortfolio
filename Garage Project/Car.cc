#include "Car.h"

Car::Car(const string &make, const string &model, const int year) : make(make), model(model), year(year) {}

void Car::print() const
{
    cout << "Make: " << make << endl;
    cout << "Model: " << model << endl;
    cout << "Year: " << year << endl;
}