#ifndef USER_H
#define USER_H

#include <string>
#include <iostream>

class User {
public:
   User(const std::string& u, const std::string& p): username(u), password(p) {};
   virtual ~User() = default;
   virtual void resetPassword(const std::string& p) {password = p;};
   virtual void print() const {
           std::cout << "User: " << username << std::endl;
       }
   virtual std::string get_username() {return username;}
   virtual std::string get_password() {return password;}
   virtual std::string getUserType() const { return "User"; }
protected:
   std::string username;
   std::string password;
};

#endif // USER_H
