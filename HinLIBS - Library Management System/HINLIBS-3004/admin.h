#ifndef ADMIN_H
#define ADMIN_H

#include <string>
#include <user.h>

class Admin : public User {
public:
    Admin(const std::string& u, const std::string& p): User(u, p) {};
    std::string getUserType() const override { return "Admin"; }
};

#endif // ADMIN_H
