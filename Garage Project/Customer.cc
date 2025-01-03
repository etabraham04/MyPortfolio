#include "Customer.h"

const char Customer::code = 'C';
int Customer::nextId = 1;

Customer::Customer(const string &name, const string &make, const string &model, const int year) : Entity(code, nextId++, name), car(make, model, year) {}

void Customer::print() const
{
    Entity::print();
    cout << "Car: " << endl;
    car.print();
}