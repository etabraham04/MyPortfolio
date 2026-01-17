#ifndef BROWSEPATRONS_H
#define BROWSEPATRONS_H

#include <QDialog>
#include <QLineEdit>
#include <QWidget>
#include <QTableWidget>

#include <dbmanager.h>

namespace Ui {
class browsePatrons;
}

class browsePatrons : public QDialog
{
    Q_OBJECT

public:
    explicit browsePatrons(QWidget *parent = nullptr, DBManager* db = nullptr);
    ~browsePatrons();
    void refresh();

signals:
    void viewAccountRequested(Patron* user);
private:
    Ui::browsePatrons *ui;
    DBManager* dbManager;
    std::vector<User*> currentUsers;
    std::vector<CatalogueItem*> currentItems;
    void loadPatrons(std::string searchParam = "");
};

#endif // BROWSEPATRONS_H
