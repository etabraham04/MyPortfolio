#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>
#include <string>

#include "Entity.h"
#include "Car.h"

using namespace std;

class Customer : public Entity
{
public:
    // constructor
    Customer(const string &name, const string &make, const string &model, int year);

    // class variables
    static const char code;
    static int nextId;
    static void resetID() { nextId = 1; };

    // member functions
    Car *getCar() { return &car; };
    virtual void print() const;

private:
    // member variables
    Car car;
};

#endif