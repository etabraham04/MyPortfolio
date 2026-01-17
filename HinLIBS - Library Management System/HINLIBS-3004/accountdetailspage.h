#ifndef ACCOUNTDETAILSPAGE_H
#define ACCOUNTDETAILSPAGE_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QDate>

#include <catalogueItem.h>
#include  <dbmanager.h>


namespace Ui {
class AccountDetailsPage;
}

class AccountDetailsPage : public QWidget
{
    Q_OBJECT

public:
    explicit AccountDetailsPage(QWidget *parent = nullptr, DBManager* db = nullptr);
    ~AccountDetailsPage();
    QPushButton* loanSelectButton() const;
    QPushButton* holdsSelectButton() const;
    CatalogueItem* on_loanSelectButton_clicked_helper();
    CatalogueItem* on_holdsSelectButton_clicked_helper();
    void setPatron(Patron* p) {
        patron = p;
        this->refresh();
    }
    // Refresh page state and data based on current logged-in user
    void refresh();

signals:
    void itemSelected(CatalogueItem* item);


private:
    Ui::AccountDetailsPage *ui;
    DBManager* dbManager;
    // Always initialize pointers; was previously uninitialized causing UB & crashes.
    Patron* patron {nullptr};

    struct HoldRow {
        CatalogueItem* item;
        int queuePosition; // 0 to n-1
        bool readyForPickup;
    };

    std::vector<HoldRow> activeHolds;
    std::vector<CatalogueItem*> activeLoans; // cached for selection by row

    //member functions
    void load_username();
    void load_user_loans();
    void load_user_holds();
};



#endif // ACCOUNTDETAILSPAGE_H
