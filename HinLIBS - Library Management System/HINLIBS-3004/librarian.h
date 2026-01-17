#ifndef LIBRARIAN_H
#define LIBRARIAN_H

#include <string>
#include <user.h>

class Librarian : public User {
public:
    Librarian(const std::string& u, const std::string& p): User(u, p) {};
    std::string getUserType() const override { return "Librarian"; }
};

#endif // LIBRARIAN_H
