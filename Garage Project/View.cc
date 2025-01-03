#include <iostream>
using namespace std;
#include <string>
#include <vector>

#include "View.h"

void View::menu(const vector<string>& menu, int& choice)
{
  cout << endl;
  cout << "Please make a selection:"<< endl<<endl;
  for (int i = 0; i < menu.size(); ++i){
    cout<<"  ("<<i+1<<") "<<menu[i]<<endl;
  }
  cout << "  (0) Exit" << endl<<endl;

  cout << "Enter your selection: ";
  cin >> choice;
  if (choice == 0)
    return;

  while (choice < 0 || choice > menu.size()) {
    cout << "Enter your selection: ";
    cin >> choice;
  }
}

void View::getId(char code, string& id)
{
  cout << "Enter an id (you may enter the number only or the complete id): ";
  cin >> id;
  if (id[0] != code){
    id = code + id;
    //cout<<"Final id: "<<id<<endl;
  }
}


