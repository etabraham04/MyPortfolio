#include "Mechanic.h"

const char Mechanic::code = 'M';
int Mechanic::nextId = 1;

Mechanic::Mechanic(const string &name) : Entity(code, nextId++, name) {}

void Mechanic::print() const
{
    cout << "Mechanic: " << endl;
    Entity::print();
}