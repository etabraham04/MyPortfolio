#include "RepairList.h"

RepairList::RepairList()
{
    this->head = nullptr;
    this->tail = nullptr;
    this->n = 0;
}

RepairList::~RepairList()
{
    Node *current = this->head;
    while (current != nullptr)
    {
        Node *next = current->next;

        delete current->data;
        delete current;

        current = next;
    }
}

void RepairList::add(Repair *repair)
{
    Node *newNode = new Node;
    newNode->data = repair;

    if (this->head == nullptr)
    {
        this->head = newNode;
        this->tail = newNode;
    }
    else
    {
        this->tail->next = newNode;
        this->tail = newNode;
    }

    n++;
}

Repair *RepairList::getNext()
{
    if (this->head == nullptr)
    {
        return nullptr;
    }

    Node *current = this->head;
    Repair *repair = current->data;

    this->head = this->head->next;

    // if head is now nullptr, tail should be nullptr as well
    if (this->head == nullptr)
    {
        this->tail = nullptr;
    }

    delete current;
    n--;
    return repair;
}

bool RepairList::isEmpty() const
{

    return n == 0;
}

int RepairList::size() const
{
    return n;
}

void RepairList::print() const
{
    Node *current = this->head;
    while (current != nullptr)
    {
        current->data->print();
        current = current->next;
    }
}