#ifndef CAR_H
#define CAR_H

#include <string>
#include <iostream>

using namespace std;

class Car
{
public:
    Car(const string &make, const string &model, const int year);


    void print() const;

private:
    string make;
    string model;
    int year;
};

#endif