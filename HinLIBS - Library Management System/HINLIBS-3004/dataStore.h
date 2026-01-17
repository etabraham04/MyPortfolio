#ifndef DATASTORE_H
#define DATASTORE_H

#include <vector>
#include <memory>
#include <user.h>
#include <patron.h>
#include <catalogue.h>
#include <iostream>

class DataStore {
public:
    DataStore() = default;
    void addUser(std::unique_ptr<User> user) {
            users.push_back(std::move(user));
        }
    std::vector<std::unique_ptr<User>>& getUsers() { return users; }
    Catalogue& getCatalogue() { return catalogue; }
    User* getUser(std::string uname){
        for (const std::unique_ptr<User>& user:  users){
            if (user->checkCreds(uname)){
                return user.get();
            }
        }
        return nullptr;
    }

private:
    std::vector<std::unique_ptr<User>> users;
    Catalogue catalogue;
};

#endif // DATASTORE_H
