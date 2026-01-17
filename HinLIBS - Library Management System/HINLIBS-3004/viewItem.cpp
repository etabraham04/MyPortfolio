#include "viewItem.h"
#include "ui_viewItem.h"

ViewItem::ViewItem(QWidget *parent, DBManager* db)
    : QWidget(parent), ui(new Ui::viewitem), dbManager(db)
{
    ui->setupUi(this);

    // Initialize pointer to null to avoid undefined behavior
    currentItem = nullptr;

    connect(ui->checkOutButton, &QPushButton::clicked, this, &ViewItem::handleCheckOut);
    connect(ui->checkInButton, &QPushButton::clicked, this, &ViewItem::handleCheckIn);
    connect(ui->applyHoldButton, &QPushButton::clicked, this, &ViewItem::handleApplyHold);
    connect(ui->cancelHoldButton, &QPushButton::clicked, this, &ViewItem::handleCancelHold);

    ui->checkOutButton->hide();
    ui->checkInButton->hide();
    ui->applyHoldButton->hide();
    ui->cancelHoldButton->hide();

    if (QWidget* buttonRow = this->findChild<QWidget*>("buttonRow")) {
        QHBoxLayout* layout = qobject_cast<QHBoxLayout*>(buttonRow->layout());
        if (layout) {
            circulationToggleButton = new QPushButton(buttonRow);
            circulationToggleButton->setObjectName("circulationToggleButton");
            circulationToggleButton->setStyleSheet(ui->checkOutButton->styleSheet());
            circulationToggleButton->hide();
            layout->addWidget(circulationToggleButton);
            connect(circulationToggleButton, &QPushButton::clicked, this, &ViewItem::handleToggleCirculation);
        }
    }
}

ViewItem::~ViewItem() {
    if (currentItem) {
        delete currentItem;
    }
    delete ui;
}

void ViewItem::setItem(CatalogueItem* item) {
    if (currentItem) {
        delete currentItem;
        currentItem = nullptr;
    }

    if (!item || !dbManager) {
        return;
    }

    try {
        currentItem = dbManager->getItemByIsbn(item->get_isbn());
    } catch (...) {
        currentItem = nullptr;
    }

    loadItemDetails();
    updateButtons();
}

void ViewItem::setUser(User* user) {
    loggedUser = user;
    updateButtons();
}

void ViewItem::loadItemDetails() {
    if (!currentItem) return;
    if (!dbManager) return;

    try {
        Patron* patron = dynamic_cast<Patron*>(loggedUser);

        ui->titleLabel->setText(QString::fromStdString(currentItem->get_title()));
        ui->authorLabel->setText(QString::fromStdString("Author: "+currentItem->get_author()));
        ui->formatLabel->setText(QString::fromStdString("Format: "+currentItem->get_format()));

        std::string status;
        Loan* loanPtr = dbManager->getLoan(currentItem);
        if (loanPtr && loanPtr->getLoanStatus() == LoanStatus::Loaned) {
            status = "Loaned";
        } else if (dbManager->hasHolds(currentItem) && patron && !(dbManager->getHoldPosition(currentItem, patron) != 0)) {
            status = "On Hold";
        } else {
            status = dbManager->getItemStatus(currentItem);
        }
        ui->statusLabel->setText(QString::fromStdString(status));
        delete loanPtr;

    } catch (const std::exception& e) {
    } catch (...) {
    }
}

void ViewItem::updateButtons() {
    if (!currentItem || !loggedUser) {
        return;
    }

    if (dynamic_cast<Librarian*>(loggedUser)) {
        ui->checkOutButton->hide();
        ui->checkInButton->hide();
        ui->applyHoldButton->hide();
        ui->cancelHoldButton->hide();
        if (circulationToggleButton) {
            std::string baseStatus = currentItem->get_circulationStatus();
            if (dbManager) {
                std::string dbStatus = dbManager->getItemStatus(currentItem);
                if (!dbStatus.empty()) baseStatus = dbStatus;
            }

            bool inCirculation = (baseStatus != "Removed");
            circulationToggleButton->setText(inCirculation ? "remove from catalogue circulation" : "add to catalogue circulation");

            bool canRemove = true;
            if (inCirculation) {
                bool isAvailableStatus = (baseStatus == "Available");
                bool isLoaned = false;
                bool hasHold = false;
                if (dbManager) {
                    Loan* loanPtr = dbManager->getLoan(currentItem);
                    isLoaned = (loanPtr && loanPtr->getLoanStatus() == LoanStatus::Loaned);
                    if (loanPtr) { delete loanPtr; loanPtr = nullptr; }
                    hasHold = dbManager->hasHolds(currentItem);
                }
                canRemove = isAvailableStatus && !isLoaned && !hasHold;
            }
            circulationToggleButton->setEnabled(inCirculation ? canRemove : true);
            circulationToggleButton->setToolTip((inCirculation && !circulationToggleButton->isEnabled())
                                                ? "Can only remove items that are Available with no active loans or holds"
                                                : "");
            circulationToggleButton->show();
        }
        return;
    }

    Patron* patron = dynamic_cast<Patron*>(loggedUser);
    if (!patron) {
        ui->checkOutButton->setEnabled(false);
        ui->checkInButton->setEnabled(false);
        ui->applyHoldButton->setEnabled(false);
        ui->cancelHoldButton->setEnabled(false);
        if (circulationToggleButton) circulationToggleButton->hide();
        return;
    }

    if (circulationToggleButton) circulationToggleButton->hide();
    ui->checkOutButton->show();
    ui->checkInButton->show();
    ui->applyHoldButton->show();
    ui->cancelHoldButton->show();

    Loan* loanPtr = dbManager->getLoan(currentItem);
    bool itemAvailable = loanPtr ? (loanPtr->getLoanStatus() == LoanStatus::Available) : true;
    bool hasHold = (dbManager->getHoldPosition(currentItem, patron) != -1);
    bool userCanBorrow = (dbManager->getLoanedItems(patron).size() < 3);

    bool itemOnHold = (dbManager->hasHolds(currentItem) && dbManager->getHoldPosition(currentItem, patron) != 0);

    bool userHasItem = false;
    std::vector<CatalogueItem*> loanedItems = dbManager->getLoanedItems(patron);
    for (CatalogueItem* loanedItem : loanedItems) {
        if (loanedItem->get_isbn() == currentItem->get_isbn()) {
            userHasItem = true;
        }
        delete loanedItem;
    }

    ui->checkOutButton->setEnabled(itemAvailable && userCanBorrow && !itemOnHold);
    ui->checkInButton->setEnabled(userHasItem);

    ui->applyHoldButton->setEnabled(!hasHold && !userHasItem && (!itemAvailable || itemOnHold));
    ui->cancelHoldButton->setEnabled(hasHold);
    if (loanPtr) { delete loanPtr; loanPtr = nullptr; }
}

void ViewItem::handleCheckOut() {
    Patron* patron = dynamic_cast<Patron*>(loggedUser);
    if (!currentItem || !patron) return;

    dbManager->checkOut(currentItem, patron);
    dbManager->cancelHold(currentItem, patron);

    loadItemDetails();
    updateButtons();

    emit itemUpdated();
}

void ViewItem::handleCheckIn() {
    Loan* loan = dbManager->getLoan(currentItem);
    if (!loan || loan->getLoanStatus() != LoanStatus::Loaned) {
        delete loan;
        return;
    }

    std::string borrowerUsername = dbManager->getLoanBorrower(currentItem);
    delete loan;

    if (borrowerUsername.empty()) return;

    Patron* borrower = dynamic_cast<Patron*>(dbManager->getUser(borrowerUsername));
    if (!borrower) return;

    dbManager->checkIn(currentItem, borrower);
    delete borrower;

    loadItemDetails();
    updateButtons();
    emit itemUpdated();
}

void ViewItem::handleApplyHold() {
    Patron* patron = dynamic_cast<Patron*>(loggedUser);
    if (!currentItem || !patron) return;

    dbManager->applyHold(currentItem, patron);
    updateButtons();
    emit itemUpdated();
}

void ViewItem::handleCancelHold() {
    Patron* patron = dynamic_cast<Patron*>(loggedUser);
    if (!currentItem || !patron) return;

    dbManager->cancelHold(currentItem, patron);
    loadItemDetails();
    updateButtons();
    emit itemUpdated();
}

void ViewItem::handleToggleCirculation() {
    if (!currentItem) return;
    if (!loggedUser || !dynamic_cast<Librarian*>(loggedUser)) return;
    if (!dbManager) return;
    try {
        std::string baseStatus = currentItem->get_circulationStatus();
        {
            std::string dbStatus = dbManager->getItemStatus(currentItem);
            if (!dbStatus.empty()) baseStatus = dbStatus;
        }
        const bool inCirculation = (baseStatus != "Removed");
        const std::string newStatus = inCirculation ? "Removed" : "Available";

        if (inCirculation) {
            bool isLoaned = false;
            bool hasHold = false;
            Loan* loanPtr = dbManager->getLoan(currentItem);
            isLoaned = (loanPtr && loanPtr->getLoanStatus() == LoanStatus::Loaned);
            if (loanPtr) { delete loanPtr; loanPtr = nullptr; }
            hasHold = dbManager->hasHolds(currentItem);

            if (baseStatus != "Available" || isLoaned || hasHold) {
                return;
            }
        }
        if (!dbManager->updateItemStatus(currentItem, newStatus)) {
            qDebug() << "ToggleCirculation: db update failed";
        }
        currentItem->set_circulationStatus(newStatus);
        if (ui->statusLabel) ui->statusLabel->setText(QString::fromStdString(newStatus));
        if (circulationToggleButton) {
            circulationToggleButton->setText(newStatus == "Removed" ? "add to catalogue circulation" : "remove from catalogue circulation");
            if (newStatus == "Removed") {
                circulationToggleButton->setEnabled(true);
                circulationToggleButton->setToolTip("");
            } else {
                circulationToggleButton->setEnabled(true);
                circulationToggleButton->setToolTip("");
            }
        }
        updateButtons();
        emit itemUpdated();
    } catch (const std::exception& e) {
        qDebug() << "ToggleCirculation: exception" << e.what();
    } catch (...) {
        qDebug() << "ToggleCirculation: unknown exception";
    }
}
