#include "Entity.h"

Entity::Entity(const char code, const int num, const string &name) : name(name)
{
    id = code + to_string(num);
    repairs = new RepairList();
};

Entity::~Entity()
{
    delete repairs;
}

bool Entity::hasRepairs() const
{
    return !repairs->isEmpty();
}

void Entity::addRepair(Repair *repair)
{
    repairs->add(repair);
}

Repair *Entity::getNextRepair()
{
    return repairs->getNext();
}

void Entity::print() const
{
    cout << "Name: " << name << endl;
    cout << "ID: " << id << endl;
    cout << "# of Repairs: " << repairs->size() << endl;
}

void Entity::printRepairs() const
{
    print();
    repairs->print();
}