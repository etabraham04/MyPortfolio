#include "Repair.h"
#include "Garage.h"

// set NOTCOMPLETE to "not complete"
const string Repair::NOTCOMPLETE = "not complete";

Repair::Repair(int repairCode, double cost)
{
    this->repairCode = repairCode;
    this->cost = cost;
    this->mechanicID = Repair::NOTCOMPLETE;
}

// member functions
bool Repair::isComplete() const
{
    return this->mechanicID != Repair::NOTCOMPLETE;
}

void Repair::completeRepair(const string &mechanicID)
{
    this->mechanicID = mechanicID;
}

void Repair::print() const
{
    cout << "Repair Code: " << this->repairCode << endl;
    cout << "Repair Description: " << Garage::REPAIRS[this->repairCode] << endl;
    cout << "Cost: " << this->cost << endl;
    cout << "Mechanic ID: " << this->mechanicID << endl;
}