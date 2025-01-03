#ifndef REPAIR_H
#define REPAIR_H

#include <iostream>
#include <string>

using namespace std;

class Repair
{
public:
    // constructor
    Repair(const int repairCode, const double cost);

    // static member variables
    static const string NOTCOMPLETE;

    // member functions
    bool isComplete() const;
    void completeRepair(const string &mechanicID);
    void print() const;

private:
    // member variables
    int repairCode;
    double cost;
    string mechanicID;
};

#endif