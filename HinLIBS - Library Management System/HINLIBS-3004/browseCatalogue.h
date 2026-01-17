#ifndef BROWSECATALOGUE_H
#define BROWSECATALOGUE_H

#include <QWidget>
#include <QLineEdit>

#include <dbmanager.h>

class CatalogueItem;

namespace Ui {
class BrowseCatalogue;
}

class BrowseCatalogue : public QWidget
{
    Q_OBJECT

public:
    explicit BrowseCatalogue(QWidget *parent = nullptr, DBManager* db = nullptr);
    ~BrowseCatalogue();
    void refresh(User* currentUser);

signals:
    void viewItemRequested(CatalogueItem* item);

private:
    Ui::BrowseCatalogue *ui;
    DBManager* dbManager;
    void loadCatalogueItems(User* currentUser);
    std::vector<CatalogueItem*> loadedItems;
};

#endif // BROWSECATALOGUE_H

