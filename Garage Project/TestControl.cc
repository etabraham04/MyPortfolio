
#include "TestControl.h"

// int printCustomersTest();
// int printMechanicsTest();
// int addRepairTest();
// int makeRepairTest();


const vector<string> TestControl::menu ={
    "Print customer test",
    "Print mechanic test",
    "Add repair test",
    "Make repair test\n",
    "Test all and get mark\n"
};


void TestControl::launch(){
    //get input, launch the appropriate test
    int choice = -1;

    while (choice!= 0){
        view.menu(menu, choice);
        switch(choice){
            case 1: printCustomersTest(); break;
            case 2: printMechanicsTest(); break;
            case 3: addRepairTest(); break;
            case 4: makeRepairTest(); break;
            case 5: int mark = 0;
                    mark += printCustomersTest();
                    mark += printMechanicsTest();
                    mark += addRepairTest();
                    mark += makeRepairTest();
                    cout<<endl<<"Your mark is "<<mark<<"/40"<<endl;
                    break;
        }
    }
}

void TestControl::initGarage(Garage& garage){
    cout<<"Initializing Garage..."<<endl;
    garage.resetIds();
    for (int i = 0; i < 3; i++){
        garage.addMechanic(mechanics[i]);
    }
    for (int i = 0; i < 4; i++){
        garage.addCustomer(customers[i], makes[i], models[i], years[i]);
    }
  
}

int TestControl::printCustomersTest(){
    cout<<"Testing printCustomers..."<<endl;
    Garage garage;
    initGarage(garage);
    tester.initCapture();
    garage.printCustomers();
    tester.endCapture();

    int error = 0;
    int mark = 0;
    tester.find(customers, error);
    if (error == 0){
        mark += 2;
        cout<<"Customer names found, 2 marks"<<endl;
    }else{
        cout<<"Customer names not found"<<endl;
    }

    tester.find(makes, error);
    if (error == 0){
        mark += 2;
        cout<<"Vehicle make found, 2 marks"<<endl;
    }else{
        cout<<"Vehicle make not found"<<endl;
    }

    tester.find(models, error);
    if (error == 0){
        mark += 2;
        cout<<"Vehicle model found, 2 marks"<<endl;
    }else{
        cout<<"Vehicle model not found"<<endl;
    }

    tester.find(yearsStr, error);
    if (error == 0){
        mark += 2;
        cout<<"Vehicle year found, 2 marks"<<endl;
    }else{
        cout<<"Vehicle year not found"<<endl;
    }

    if (mark == 8){
        cout<<"All customer information found"<<endl;
    }
    cout<<"Tests complete, mark is "<<mark<<"/8"<<endl;

    return mark;
}

int TestControl::printMechanicsTest(){
    cout<<"Testing printMechanics..."<<endl;
    Garage garage;
    initGarage(garage);
    tester.initCapture();
    garage.printMechanics();
    tester.endCapture();

    int error = 0;
    int mark = 0;
    tester.find(mechanics, error);
    if (error == 0){
        mark += 4;
        cout<<"Mechanic names found, 4 marks"<<endl;
    }else{
        cout<<"Mechanic names not found"<<endl;
    }

    cout<<"Tests complete, mark is "<<mark<<"/4"<<endl;

    return mark;
}

int TestControl::findCustomer(int index){
    int error = 0;
    int mark = 0;
    tester.find({index}, customers, error);
    if (error == 0){
        mark += 1;
        cout<<"Customer name found, 1 mark"<<endl;
    }else{
        cout<<"Customer name "<<customers[index]<<" not found"<<endl;
    }

    tester.find({index}, makes, error);
    if (error == 0){
        mark += 1;
        cout<<"Vehicle make found, 1 mark"<<endl;
    }else{
        cout<<"Vehicle make "<<makes[index]<<" not found"<<endl;
    }

    tester.find({index}, models, error);
    if (error == 0){
        mark += 1;
        cout<<"Vehicle model found, 1 mark"<<endl;
    }else{
        cout<<"Vehicle model "<<models[index]<<" not found"<<endl;
    }

    tester.find({index}, yearsStr, error);
    if (error == 0){
        mark += 1;
        cout<<"Vehicle year found, 1 mark"<<endl;
    }else{
        cout<<"Vehicle year "<<yearsStr[index]<<" not found"<<endl;
    }

    return mark;
}

int TestControl::addRepairTest(){
    cout<<"Testing addRepair..."<<endl;
    Garage garage;
    initGarage(garage);
    vector<int> repairCodes = {5, 1, 7, 3};
    vector<string> custIds = {"C1", "C3", "C3", "C1"};
    for (int i = 0; i < 4; i++){
        garage.addRepair(custIds[i], repairCodes[i], costs[repairCodes[i]]);
    }

    tester.initCapture();
    garage.printCustomerRepairs("C1");
    tester.endCapture();

    cout<<"Checking for customer C1 information..."<<endl;

    int mark = findCustomer(0);
    
    int error = 0;
    cout<<"Checking for C1 repair information..."<<endl;

    tester.find({5,3},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"Repairs found, 2 marks"<<endl;
    }else{
        cout<<"Repairs not found"<<endl;
    }

    // look for wrong repairs
    tester.confirmAbsent({0,1,2,4,6,7,8,9},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"No wrong repairs found, 2 marks"<<endl;
    }else{
        cout<<"Wrong repairs found"<<endl;
    }

    cout<<"Checking C3 repair information"<<endl;
    tester.initCapture();
    garage.printCustomerRepairs("C3");
    tester.endCapture();

    tester.find({1,7},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"Repairs found, 2 marks"<<endl;
    }else{
        cout<<"Repairs not found"<<endl;
    }

    // look for wrong repairs
    tester.confirmAbsent({0,2,3,4,5,6,8,9},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"No wrong repairs found, 2 marks"<<endl;
    }else{
        cout<<"Wrong repairs found"<<endl;
    }
    cout<<"Tests complete, mark is "<<mark<<"/12"<<endl;
    
    return mark;
}

int TestControl::makeRepairTest(){
    cout<<"Testing makeRepair..."<<endl;
    Garage garage;
    initGarage(garage);
    vector<int> repairCodes = {5, 1, 7, 3};
    vector<string> custIds = {"C1", "C3", "C3", "C1"};
    for (int i = 0; i < 4; i++){
        garage.addRepair(custIds[i], repairCodes[i], costs[repairCodes[i]]);
    }
    garage.makeNextRepair("M2", "C3");

    /*
    * We are going to be careful here. One repair should be moved from C1 to M2
    */

    tester.initCapture();
    garage.printMechanicRepairs("M2");
    tester.endCapture();

    int mark = 0;
    int error = 0;

    cout<<"Checking for mechanic M2 information..."<<endl;
    
    tester.find({1}, mechanics, error);
    if (error == 0){
        mark += 2;
        cout<<"Mechanic name found, 2 marks"<<endl;
    }else{
        cout<<"Mechanic name not found"<<endl;
    }

    cout<<"Checking for M2 repair information..."<<endl;

    tester.find({1},Garage::REPAIRS, error);

    if (error == 0){
        mark += 2;
        cout<<"Repairs found, 2 marks"<<endl;
    }else{
        cout<<"Repairs not found"<<endl;
    }

    // look for wrong repairs
    tester.confirmAbsent({0,2,3,4,5,6,7,8,9},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"No wrong repairs found, 2 marks"<<endl;
    }else{
        cout<<"Wrong repairs found"<<endl;
    }

    cout<<"Checking C3 repair information"<<endl;
    tester.initCapture();
    garage.printCustomerRepairs("C3");
    tester.endCapture();

    tester.find({7},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"Customer repairs found, 2 marks"<<endl;
    }else{
        cout<<"Customer repairs not found"<<endl;
    }

    // look for wrong repairs
    tester.confirmAbsent({0,1,2,3,4,5,6,8,9},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"No wrong repairs found, 2 marks"<<endl;
    }else{
        cout<<"Wrong repairs found"<<endl;
    }

    cout<<"Repairing C1 three times (only two repairs available)"<<endl;
    garage.makeNextRepair("M1", "C1");
    garage.makeNextRepair("M1", "C1");
    garage.makeNextRepair("M1", "C1");

    tester.initCapture();
    garage.printMechanicRepairs("M1");
    tester.endCapture();

    cout<<"Checking for M2 repair information..."<<endl;

    tester.find({5,3},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"Repairs found, 2 marks"<<endl;
    }else{
        cout<<"Repairs not found"<<endl;
    }

    // look for wrong repairs
    tester.confirmAbsent({0,1,2,4,6,7,8,9},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"No wrong repairs found, 2 marks"<<endl;
    }else{
        cout<<"Wrong repairs found"<<endl;
    }

    cout<<"Checking C1 repair information"<<endl;
    tester.initCapture();
    garage.printCustomerRepairs("C1");
    tester.endCapture();

    tester.confirmAbsent({0,1,2,3,4,5,6,7,8,9},Garage::REPAIRS, error);
    if (error == 0){
        mark += 2;
        cout<<"No wrong repairs found, 2 marks"<<endl;
    }else{
        cout<<"Wrong repairs found"<<endl;
    }
    

    cout<<"Tests complete, mark is "<<mark<<"/16"<<endl;
    return mark;

}


vector<string> TestControl::mechanics = {
    "Bob",
    "Sue",
    "Joe"
};

vector<string> TestControl::customers = {
    "Mary",
    "Bill",
    "Sue",
    "Joe"
};

vector<string> TestControl::yearsStr = {
    "2018",
    "2019",
    "2017",
    "2016"
};

int TestControl::years[4] = {
    2018,
    2019,
    2017,
    2016
};

vector<string> TestControl::makes = {
    "Ford",
    "Chevy",
    "Toyota",
    "Honda"
};

vector<string> TestControl::models = {
    "F150",
    "Silverado",
    "Camry",
    "Civic"
};


const double TestControl::costs[10] = {
    100.00,
    200.00,
    300.00,
    400.00,
    500.00,
    600.00,
    700.00,
    800.00,
    900.00,
    1000.00
};
