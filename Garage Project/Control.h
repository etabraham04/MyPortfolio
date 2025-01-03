
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <iostream>
#include <string>
#include <random>
#include <unordered_set>
#include <sstream>
#include "Garage.h"
#include "View.h"
#include "Tester.h"

using namespace std;

class Control {
		
	public:
		
		void launch();

	
	private:
		void initGarage();

		void printMechanics();
		void printCustomers();

		void printCustomerRepairs();
		void printMechanicRepairs();

		void inspectVehicle();
		void assignMechanic();
		
		
		Garage garage;
		View view;
		Tester tester;

		static const vector<string> menu;

};
#endif