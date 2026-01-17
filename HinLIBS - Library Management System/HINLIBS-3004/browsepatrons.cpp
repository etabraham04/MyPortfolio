#include "browsepatrons.h"
#include "ui_browsepatrons.h"


#include <QObject>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QDate>
#include <QHeaderView>
#include <magazine.h>
#include <QMessageBox>


browsePatrons::browsePatrons(QWidget *parent, DBManager* db) :
    QDialog(parent), ui(new Ui::browsePatrons), dbManager(db)
{
    ui->setupUi(this);
    refresh();
    connect(ui->searchButton, &QPushButton::clicked, this, [this]()
    {
        loadPatrons(ui->searchBar->text().toStdString());
    });
}

browsePatrons::~browsePatrons()
{
    for (CatalogueItem* item : currentItems) {
        delete item;
    }
    currentItems.clear();


    for (User* user : currentUsers) {
        delete user;
    }
    currentUsers.clear();

    delete ui;
}

void browsePatrons::refresh() {
    ui->searchBar->setPlaceholderText("Search here...");
    loadPatrons();
}

void browsePatrons::loadPatrons(std::string searchParam)
{

    // Get the scroll area
    QScrollArea* scrollArea = findChild<QScrollArea*>("scrollArea");
    QWidget* scrollWidget = scrollArea->widget();

    // Clear existing layout
    if (scrollWidget->layout()) {
        QLayoutItem* child;
        while ((child = scrollWidget->layout()->takeAt(0)) != nullptr) {
            delete child->widget();
            delete child;
        }
        delete scrollWidget->layout();
    }

    // Create new layout for items
    QVBoxLayout* itemsLayout = new QVBoxLayout(scrollWidget);

    for (CatalogueItem* item : currentItems) {
        delete item;
    }
    currentItems.clear();

    for (User* user : currentUsers) {
        delete user;
    }
    currentUsers.clear();

    if (searchParam != ""){
        currentUsers = dbManager->getUsersContaining(searchParam);
    } else {
        currentUsers = dbManager->getAllUsers();
    }

    for (User* user : currentUsers) {
        if (user->getUserType() == "Patron") {

            QWidget* userWidget = new QWidget();
            userWidget->setStyleSheet("background-color: #9ebe41; border-radius: 10px; margin: 1px; padding: 20px 10px; font-family: Deja Vu Serif");
            userWidget->setMinimumHeight(180);

            QHBoxLayout* userLayout = new QHBoxLayout(userWidget);

            QVBoxLayout* infoLayout = new QVBoxLayout();

            QLabel* usernameLabel = new QLabel(QString::fromStdString(user->get_username()));
            usernameLabel->setStyleSheet("font-weight: bold; font-size: 11pt; color: #364e1b; max-width: 130px; min-width: 130px; margin-bottom: -10px;");
            infoLayout->addWidget(usernameLabel);

            // View Account button
            QPushButton* viewAccount = new QPushButton("View Account");
            viewAccount->setStyleSheet(
                "QPushButton {"
                "    background-color: #384b06;"
                "    color: white;"
                "    border-radius: 5px;"
                "    font-size: 11px;"
                "    font-weight: bold;"
                "    border: 2px solid #2a3a05;"
                        "max-width: 100px;"
                        "padding: 8px 8px;"
                        "margin-left: 10px;"
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
            viewAccount->setCursor(Qt::PointingHandCursor); // Shows the hand cursor when hovered over button

            connect(viewAccount, &QPushButton::clicked, this, [this, user]() {
                emit viewAccountRequested(dynamic_cast<Patron*>(user));
            });

            std::vector<CatalogueItem*> loansVector = dbManager->getLoanedItems(dynamic_cast<Patron*>(user));

            currentItems.insert(currentItems.end(), loansVector.begin(), loansVector.end());


            QTableWidget* loansTableWidget = new QTableWidget();
            loansTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
            loansTableWidget->setColumnCount(3);
            loansTableWidget->verticalHeader()->setVisible(false);

            loansTableWidget->setShowGrid(true);
            loansTableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            loansTableWidget->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
            QHeaderView* header = loansTableWidget->horizontalHeader();
            header->setSectionResizeMode(QHeaderView::Fixed);
            header->setStretchLastSection(true);

            loansTableWidget->horizontalHeader()->setVisible(true);
            loansTableWidget->setRowCount(1);
            loansTableWidget->setColumnWidth(0, 130);
            QTableWidgetItem* titleItem = new QTableWidgetItem("Title");
            titleItem->setForeground(QBrush(QColor(0, 0, 0)));
            loansTableWidget->setItem(0, 0, titleItem);
            QTableWidgetItem* dueDateItem = new QTableWidgetItem("Due Date");
            dueDateItem->setForeground(QBrush(QColor(0, 0, 0)));
            loansTableWidget->setItem(0, 1, dueDateItem);
            QTableWidgetItem* checkoutDateItem = new QTableWidgetItem("Checkout Date");
            checkoutDateItem->setForeground(QBrush(QColor(0, 0, 0)));
            loansTableWidget->setItem(0, 2, checkoutDateItem);
            loansTableWidget->horizontalHeader()->setVisible(false);


            if (loansVector.empty()) {
                loansTableWidget->setRowCount(2);
                QTableWidgetItem* item = new QTableWidgetItem("No active loans.");
                item->setForeground(QBrush(QColor("#000")));
                loansTableWidget->setItem(1, 0, item);
                loansTableWidget->setSpan(1, 0, 1, 3);
                loansTableWidget->setEnabled(false);
            }

            else {
                loansTableWidget->setRowCount(loansVector.size()+1);
                int row = 1;
                for (CatalogueItem* item : loansVector) {
                    QString title = QString::fromStdString(item->get_title());


                    auto* loan = dbManager->getLoan(item);
                    QString dueDateStr = QString::fromStdString(loan->getDueDate());
                    delete loan;

                    QDate dueDate = QDate::fromString(dueDateStr, "yyyy-MM-dd");
                    QDate checkoutDate = dueDate.addDays(-14);
                    loansTableWidget->setItem(row, 0, new QTableWidgetItem(title));
                    loansTableWidget->setItem(row, 1, new QTableWidgetItem(dueDateStr));
                    loansTableWidget->setItem(row, 2, new QTableWidgetItem(checkoutDate.toString("yyyy-MM-dd")));
                    row++;
                }
            }

            //view item button
            QPushButton* viewItem = new QPushButton("Check In Item");
            viewItem->setStyleSheet(
                "QPushButton {"
                "    background-color: #384b06;"
                "    color: white;"
                "    padding: 8px 6px;"
                "    border-radius: 5px;"
                "    font-size: 11px;"
                "    font-weight: bold;"
                "    border: 2px solid #2a3a05; margin: 20px;"
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
            viewItem->setCursor(Qt::PointingHandCursor);
            if (loansVector.empty()) {
                viewItem->setEnabled(false);
            }

            connect(viewItem, &QPushButton::clicked, this, [this, loansTableWidget, user, loansVector, searchParam]() {
                if (!loansTableWidget->isEnabled()) {
                    QMessageBox::information(this, "No Loans",
                                             "There are no active loans to select.");
                    return;
                }

                int selectedRow = loansTableWidget->currentRow();
                if (selectedRow == -1) {
                    QMessageBox::information(this, "No Selection",
                                             "Please select an item from the loans list first.");
                    return;
                }

                selectedRow -= 1; //account for the column headers at the top row

                if (selectedRow >= static_cast<int>(loansVector.size())) {
                    // Removed out of bounds loans debug
                    return;
                }

                dbManager->checkIn(loansVector[selectedRow], dynamic_cast<Patron*>(user));
                this->loadPatrons(searchParam);
            });


            loansTableWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            viewItem->setFixedWidth(145);

            infoLayout->addWidget(viewAccount);
            infoLayout->setAlignment(Qt::AlignRight | Qt::AlignTop);
            userLayout->addLayout(infoLayout);


            userLayout->addWidget(loansTableWidget);

            QHBoxLayout* viewItemLayout = new QHBoxLayout();
            viewItemLayout->setAlignment(Qt::AlignRight | Qt::AlignBottom);
            viewItemLayout->addWidget(viewItem);
            userLayout->addLayout(viewItemLayout);
            userWidget->setLayout(userLayout);

            itemsLayout->addWidget(userWidget);
            userLayout->setStretch(1, 2);
        }
    }

    scrollWidget->setLayout(itemsLayout);



    itemsLayout->addStretch();
}
