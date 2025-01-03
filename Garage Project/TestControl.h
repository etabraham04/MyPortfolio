#ifndef TESTCONTROL_H
#define TESTCONTROL_H

#include <iostream>
#include <string>
#include <vector>

#include "Garage.h"
#include "View.h"
#include "Tester.h"

using namespace std;


class TestControl
{
  public:
    
    void launch();
    

  private:
    int printCustomersTest();
    int printMechanicsTest();
    int addRepairTest();
    int findCustomer(int);
    int makeRepairTest();

    //helper function
    void initGarage(Garage& garage);

    View view;
    Tester tester;

    const static vector<string> menu;

    // Mechanic and Customer information
    static vector<string> mechanics;
    
    static vector<string> customers;
    static vector<string> makes;
    static vector<string> models;
    static vector<string> yearsStr;
    static int years[4];

    // Repair information
    const static double costs[10];


    
};

#endif
