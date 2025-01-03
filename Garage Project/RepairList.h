#ifndef REPAIRLIST_H
#define REPAIRLIST_H

#include <iostream>
#include <string>
#include "Repair.h"

class RepairList
{

    class Node
    {
    public:
        Repair *data;
        Node *next;
    };

public:
    // constructor
    RepairList();

    // destructor
    ~RepairList();

    // member functions
    void add(Repair *repair);
    Repair *getNext();
    bool isEmpty() const;
    int size() const;
    void print() const;

private:
    // member variables
    Node *head;
    Node *tail;
    int n; // added onto of spec
};

#endif