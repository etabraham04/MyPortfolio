
#include <iostream>
#include <string>
#include <vector>
#include "Control.h"


const vector<string> Control::menu ={
    "Print mechanics",
    "Print customers",
    "Print customer repairs to complete",
    "Print mechanic repairs completed",
    "Inspect vehicle (generate repairs)",
    "Assign mechanic"
};


void Control::launch(){
    //make a view, get input, launch the appropriate test
    View view;
    int choice = -1;

    initGarage();

    while (choice!= 0){
        view.menu(menu, choice);
        switch(choice){
            case 1: printMechanics(); break;
            case 2: printCustomers(); break;
            case 3: printCustomerRepairs(); break;
            case 4: printMechanicRepairs(); break;
            case 5: inspectVehicle(); break;
            case 6: assignMechanic(); break;
        }
    }
}

void Control::initGarage(){
    cout<<"Initializing Garage..."<<endl;
    garage.addMechanic("Bob");
    garage.addMechanic("Sue");
    garage.addMechanic("Joe");

    garage.addCustomer("Mary", "Ford", "F150", 2018);
    garage.addCustomer("Bill", "Chevy", "Silverado", 2019);
    garage.addCustomer("Sue", "Toyota", "Camry", 2017);
    garage.addCustomer("Joe", "Honda", "Civic", 2016);

   
}

void Control::printMechanics(){
    cout<<"Printing Mechanics..."<<endl;
    garage.printMechanics();
}

void Control::printCustomers(){
    cout<<"Printing Customers..."<<endl;
    garage.printCustomers();
}

void Control::printCustomerRepairs(){
    cout<<"Choose customer to print repairs for: "<<endl;
    printCustomers();
    string custId;
    view.getId('C', custId);
    cout<<"ID: "<<custId<<endl;
    garage.printCustomerRepairs(custId);
}

void Control::printMechanicRepairs(){
    cout<<"Choose mechanic to print repairs for: "<<endl;
    printMechanics();
    string mechId;
    view.getId('M', mechId);
    garage.printMechanicRepairs(mechId);
}


/* This generates some random repairs 
   and adds them to a customer
*/
void Control::inspectVehicle(){
    cout<<"Choose vehicle to inspect: "<<endl;
    printCustomers();
    string custId;
    view.getId('C', custId);
    Customer* cust = garage.getCustomer(custId);
    if (cust == nullptr){
        cout<<"Customer "<<custId<<" not found"<<endl;
        return;
    }

    int numRepairs = tester.ran(1,5);
    vector<int> repairs;
    tester.ran(repairs, numRepairs, 9);
    for (int r: repairs){
        garage.addRepair(custId, r, tester.ran(25, 150));
    }
}

void Control::assignMechanic(){
    cout<<"Please choose a mechanic to assign:"<<endl;
    garage.printMechanics();
    string mechId;
    view.getId('M', mechId);
    Mechanic* mech = garage.getMechanic(mechId);
    if (mech == nullptr){
        cout<<"Mechanic "<<mechId<<" not found"<<endl;
        return;
    }
    cout<<"Please choose a car to repair:"<<endl;
    garage.printCustomers();
    string custId;
    view.getId('C', custId);
    Customer* cust = garage.getCustomer(custId);
    if (cust == nullptr){
        cout<<"Customer "<<custId<<" not found"<<endl;
        return;
    }
    while(cust->hasRepairs()){
        garage.makeNextRepair(mechId, custId);
    }   
}


