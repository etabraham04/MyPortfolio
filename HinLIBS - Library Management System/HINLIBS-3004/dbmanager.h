#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <string>
#include <vector>
#include <patron.h>
#include <admin.h>
#include <librarian.h>
#include <loan.h>
#include <catalogueItem.h>
#include "book.h"
#include "fiction.h"
#include "nonFiction.h"
#include "magazine.h"
#include "movie.h"
#include "videoGame.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QFileInfo>
#include <QDebug>
#include <QDate>
#include <QCoreApplication>
#include <QDir>

#include <qdebug.h>

class DBManager
{
public:
    DBManager();

    bool checkIn(CatalogueItem* item, Patron* patron);
    bool checkOut(CatalogueItem* item, Patron* patron);
    std::vector<Loan*> getActiveLoans(std::string username);

    bool applyHold(CatalogueItem* item, Patron* patron);
    bool cancelHold(CatalogueItem* item, Patron* patron);
    std::vector<Patron*> getItemHolds(CatalogueItem* item);
    std::vector<CatalogueItem*> getPatronHolds(Patron* patron);
    int getHoldPosition(CatalogueItem* item, Patron* patron);
    bool hasHolds(CatalogueItem* item);
    bool readyForPickup(CatalogueItem* item, Patron* patron);

    bool addUser(Patron* patron);
    std::vector<User*>  getAllUsers();
    User* getUser(std::string usernamee);

    //return a list of users that have a username containing the string
    std::vector<User*> getUsersContaining(std::string str);

    bool addItem(CatalogueItem* item);
    std::vector<CatalogueItem*>getAllItems();
    bool updateItemStatus(CatalogueItem* item, const std::string& circulationStatus);

    Loan* getLoan(CatalogueItem* item);
    std::vector<CatalogueItem*> getLoanedItems(Patron* patron);

    std::string getItemStatus(CatalogueItem* item);
    std::string getLoanBorrower(CatalogueItem* item);

    // Fetch a single CatalogueItem by ISBN, constructing the correct concrete type.
    CatalogueItem* getItemByIsbn(int isbn);
private:
    QSqlDatabase db;
    // In-memory catalogue items for testing/filtering for librarian functionality
    std::vector<std::unique_ptr<CatalogueItem>> items;
};

#endif // DBMANAGER_H
