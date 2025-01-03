#ifndef ENTITY_H
#define ENTITY_H

#include <iostream>
#include <string>
#include "RepairList.h"

using namespace std;

class Entity
{
public:
    // constructor
    Entity(const char code, const int num, const string &name);
    ~Entity();

    // member functions
    virtual bool hasRepairs() const;
    virtual void addRepair(Repair *repair);
    virtual Repair *getNextRepair();
    virtual string getId() const { return id; };
    virtual void print() const;
    virtual void printRepairs() const;

protected:
    string name;
    string id;
    RepairList *repairs;
};

#endif