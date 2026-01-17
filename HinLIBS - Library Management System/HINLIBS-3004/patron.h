#ifndef PATRON_H
#define PATRON_H

#include <string>
#include <vector>
#include <user.h>
#include <iostream>
class CatalogueItem;

class Patron : public User {
public:
    Patron(const std::string& u, const std::string& p, const std::string& e, int cardNum)
    : User(u, p), email(e), libraryCardNumber(cardNum), borrowingPrivileges("") {}

    //member functions
    std::string getUserType() const override { return "Patron"; }
    std::string get_email() { return email; }
    int get_card_number() { return libraryCardNumber; }
private:
    std::string email;
    int libraryCardNumber;
    std::string borrowingPrivileges;
};

#endif // PATRON_H
