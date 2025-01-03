#include "Garage.h"

const string Garage::REPAIRS[NUM_REPAIRS] = {"Oil Change", "Brake Check", "Tire Rotation", "Battery Check", "Air Filter Replacement", "Coolant Flush", "Transmission Flush", "A/C Recharge", "Engine Diagnostic", "Brake Pad Replacement"};

Garage::Garage() {}
Garage::~Garage()
{
    for (Customer *customer : customers)
    {
        delete customer;
    }

    for (Mechanic *mechanic : mechanics)
    {
        delete mechanic;
    }
}

void Garage::addMechanic(const string &name)
{
    Mechanic *mechanic = new Mechanic(name);
    mechanics.push_back(mechanic);
}

void Garage::addCustomer(const string &name, const string &make, const string &model, const int year)
{
    Customer *customer = new Customer(name, make, model, year);
    customers.push_back(customer);
}

void Garage::addRepair(const string &customerId, const int repairCode, const double cost)
{
    Repair *repair = new Repair(repairCode, cost);
    Customer *customer = getCustomer(customerId);
    if (customer == nullptr)
    {
        cout << "Customer not found" << endl;
        return;
    }

    customer->addRepair(repair);
}

void Garage::makeNextRepair(const string &mechanicId, const string &customerId)
{
    Mechanic *mechanic = getMechanic(mechanicId);
    Customer *customer = getCustomer(customerId);

    if (mechanic == nullptr)
    {
        cout << "Mechanic not found" << endl;
        return;
    }

    if (customer == nullptr)
    {
        cout << "Customer not found" << endl;
        return;
    }

    Repair *repair = customer->getNextRepair();
    if (repair == nullptr){
        cout << "No more repairs for customer" << endl;
        return;
    }

    mechanic->addRepair(repair);
    repair->completeRepair(mechanic->getId());
}

Customer *Garage::getCustomer(const string &id) const
{
    for (Customer *customer : customers)
    {
        if (customer->getId() == id)
        {
            return customer;
        }
    }

    return nullptr;
}

Mechanic *Garage::getMechanic(const string &id) const
{
    for (int i = 0; i < mechanics.size(); i++)
    {
        if (mechanics[i]->getId() == id)
        {
            return mechanics[i];
        }
    }

    return nullptr;
}
void Garage::printMechanics() const
{
    for (Mechanic *mechanic : mechanics)
    {
        mechanic->print();
    }
}

void Garage::printCustomers() const
{
    for (Customer *customer : customers)
    {
        customer->print();
    }
}

void Garage::printMechanicRepairs(const string &id) const
{
    Mechanic *mechanic = getMechanic(id);
    if (mechanic == nullptr)
    {
        cout << "Mechanic not found" << endl;
        return;
    }

    mechanic->printRepairs();
}

void Garage::printCustomerRepairs(const string &id) const
{
    Customer *customer = getCustomer(id);
    if (customer == nullptr)
    {
        cout << "Customer not found" << endl;
        return;
    }

    customer->printRepairs();
}

void Garage::resetIds() const
{
    Customer::resetID();
    Mechanic::resetID();
}