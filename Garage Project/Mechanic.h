#ifndef MECHANIC_H
#define MECHANIC_H

#include <iostream>
#include <string>

#include "Entity.h"

using namespace std;

class Mechanic : public Entity
{
public:
    // constructor
    Mechanic(const string &name);

    // member functions
    virtual void print() const;

    // class variables
    static const char code;
    static int nextId;
    static void resetID() { nextId = 1; };

private:
};

#endif