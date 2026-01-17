#include "browseCatalogue.h"
#include "ui_browseCatalogue.h"
#include "catalogueItem.h"
#include "book.h"
#include <dbmanager.h>


#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

BrowseCatalogue::BrowseCatalogue(QWidget *parent, DBManager* db) :
    QWidget(parent), ui(new Ui::BrowseCatalogue), dbManager(db)
{
    ui->setupUi(this);
    loadCatalogueItems(nullptr); // original immediate load behavior
}

BrowseCatalogue::~BrowseCatalogue()
{
    for (auto item : loadedItems) {
        delete item;
    }
    loadedItems.clear();
    delete ui;
}

void BrowseCatalogue::refresh(User* currentUser) {
    ui->searchBar->setText("Search here...");
    loadCatalogueItems(currentUser);
}

void BrowseCatalogue::loadCatalogueItems(User* currentUser)
{
    if (!dbManager) return;
    qDebug() << "BrowseCatalogue: loadCatalogueItems enter";

    //Get the scroll area
    QScrollArea* scrollArea = findChild<QScrollArea*>("scrollArea");
    if (!scrollArea) {
        qDebug() << "BrowseCatalogue: scrollArea not found; aborting refresh to prevent crash.";
        return;
    }
    QWidget* scrollWidget = scrollArea->widget();
    if (!scrollWidget) {
        qDebug() << "BrowseCatalogue: scrollArea has no widget; aborting refresh to prevent crash.";
        return;
    }


    // Reuse or create items layout
    QVBoxLayout* itemsLayout = nullptr;
    if (!scrollWidget->layout()) {
        itemsLayout = new QVBoxLayout(scrollWidget);
    } else {
        itemsLayout = qobject_cast<QVBoxLayout*>(scrollWidget->layout());
        if (!itemsLayout) {
            // fallback: replace non-VBox layout
            delete scrollWidget->layout();
            itemsLayout = new QVBoxLayout(scrollWidget);
        }
        // Clear existing children only
        while (QLayoutItem* child = itemsLayout->takeAt(0)) {
            if (QWidget* w = child->widget()) {
                w->deleteLater();
            }
            delete child;
        }
    }

    // Clear old items
    for (auto item : loadedItems) {
        delete item;
    }
    loadedItems.clear();

    //Load real catalogue items
    std::vector<CatalogueItem*> allItems = dbManager->getAllItems();
    loadedItems = allItems; // Take ownership

    bool isLibrarian = currentUser && QString::fromStdString(currentUser->getUserType()).toLower() == "librarian";

    for (CatalogueItem* item : allItems) {
        qDebug() << "BrowseCatalogue: rendering item" << item->get_isbn();
        // Librarian sees all items; patrons/admins only see items with circulation_status == Available
        if (!isLibrarian && item->get_circulationStatus() != "Available") {
            continue; // skip non-available items for non-librarians
        }

            //Create item widget
            QWidget* itemWidget = new QWidget();
            itemWidget->setStyleSheet("background-color: #9ebe41; border-radius: 10px; margin: 1px; padding: 20px; font-family: Deja Vu Serif");
            itemWidget->setMinimumHeight(100);

            QHBoxLayout* itemLayout = new QHBoxLayout(itemWidget);

            //Item info
            QVBoxLayout* infoLayout = new QVBoxLayout();

            QLabel* titleLabel = new QLabel(QString::fromStdString(item->get_title()));
            QLabel* authorLabel = new QLabel(QString::fromStdString("By: " + item->get_author()));
            QLabel* formatLabel = new QLabel(QString::fromStdString("Format: " + item->get_format()));

            std::string status;
            Loan* loanPtr = dbManager->getLoan(item);
            qDebug() << "BrowseCatalogue: loanPtr acquired" << (loanPtr ? 1 : 0);
            // For librarians, show explicit 'Removed' even if other heuristics would say Available/On Hold
            if (isLibrarian && item->get_circulationStatus() == "Removed") {
                status = "Removed";
            } else if (loanPtr && loanPtr->getLoanStatus() == LoanStatus::Loaned) {
                status = "Loaned";
            } else if (dbManager->hasHolds(item)) {
                if (currentUser && dbManager->readyForPickup(item, dynamic_cast<Patron*>(currentUser))){
                        status =  "Available";
                } else {
                    status = "On Hold";
                }

            } else {
                status = "Available";
            }
            // free loanPtr if allocated (nullptr safe)
            delete loanPtr;
            QLabel* statusLabel = new QLabel(QString::fromStdString("Status: " + status));

            titleLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: #364e1b;");
            authorLabel->setStyleSheet("color: #364e1b;");
            formatLabel->setStyleSheet("color: #364e1b;");
            statusLabel->setStyleSheet(item->get_circulationStatus() == "Available" ? "color: green; font-weight: bold;" : "color: red; font-weight: bold;");

            infoLayout->addWidget(titleLabel);
            infoLayout->addWidget(authorLabel);
            infoLayout->addWidget(formatLabel);
            infoLayout->addWidget(statusLabel);

            //View Itme button
            QPushButton* viewButton = new QPushButton("View Item");
            viewButton->setStyleSheet(
                "QPushButton {"
                "    background-color: #384b06;"
                "    color: white;"
                "    padding: 8px 12px;"
                "    border-radius: 5px;"
                "    font-size: 11px;"
                "    font-weight: bold;"
                "    border: 2px solid #2a3a05;"
                "}"
                "QPushButton:hover {"
                "    background-color: #2a3a05;"
                "    border: 2px solid #1a2a03;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #1a2a03;"
                "    padding: 7px 11px;"
                "}"
            );

            viewButton->setFixedSize(90, 35);
            viewButton->setCursor(Qt::PointingHandCursor); //Shows ur hand cursor when hovered over button

            connect(viewButton, &QPushButton::clicked, this, [this, item]() {
                emit viewItemRequested(item);
            });

            itemLayout->addLayout(infoLayout);
            itemLayout->addWidget(viewButton);
            itemsLayout->addWidget(itemWidget);
        }

    if (allItems.empty()) {
        QLabel* noItemsLabel = new QLabel("No items in catalogue.");
        noItemsLabel->setAlignment(Qt::AlignCenter);
        noItemsLabel->setStyleSheet("color: #364e1b; font-weight: bold; font-size: 16pt;");
        itemsLayout->addWidget(noItemsLabel);
    }

    itemsLayout->addStretch();
    qDebug() << "BrowseCatalogue: loadCatalogueItems exit";
}
