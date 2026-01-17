#ifndef VIEWITEM_H
#define VIEWITEM_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDebug>
#include "catalogueItem.h"
#include "user.h"
#include "itemcontrol.h"
#include <dbmanager.h>

namespace Ui{
class viewitem;

}

class ViewItem : public QWidget {
    Q_OBJECT

public:
    explicit ViewItem(QWidget *parent = nullptr, DBManager* db = nullptr);
    ~ViewItem();

    void setItem(CatalogueItem* item);
    void setUser(User* user);

signals:
   void itemUpdated();

private:
    Ui::viewitem *ui;
    CatalogueItem* currentItem = nullptr;
    User* loggedUser = nullptr;
    DBManager* dbManager;
    QPushButton* circulationToggleButton = nullptr; // librarian-only button

    itemcontrol controller;

    void updateButtons();
    void loadItemDetails();

    void handleCheckOut();
    void handleCheckIn();
    void handleApplyHold();
    void handleCancelHold();
    void handleToggleCirculation();
};

#endif // VIEWITEM_H
