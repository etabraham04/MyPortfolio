#ifndef GARAGE_H
#define GARAGE_H

#include <iostream>
#include <string>
#include <vector>

#include "Customer.h"
#include "Mechanic.h"

using namespace std;

class Garage
{

public:
	static const int NUM_REPAIRS = 10;
	static const string REPAIRS[NUM_REPAIRS];

	// constructor
	Garage();
	~Garage();

	// member functions
	void addMechanic(const string &name);
	void addCustomer(const string &name, const string &make, const string &model, const int year);
	void addRepair(const string &customerId, const int repairCode, const double cost);
	void makeNextRepair(const string &mechanicId, const string &customerId);
	Customer *getCustomer(const string &id) const;
	Mechanic *getMechanic(const string &id) const;
	void printMechanics() const;
	void printCustomers() const;
	void printMechanicRepairs(const string &id) const;
	void printCustomerRepairs(const string &id) const;
	void resetIds() const;

private:
	// memeber variables
	vector<Customer *> customers;
	vector<Mechanic *> mechanics;
};
#endif