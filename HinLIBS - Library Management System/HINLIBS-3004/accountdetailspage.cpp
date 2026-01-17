#include "accountdetailspage.h"
#include "ui_accountdetailspage.h"

AccountDetailsPage::AccountDetailsPage(QWidget *parent, DBManager* db) :
    QWidget(parent),
    ui(new Ui::AccountDetailsPage),
    dbManager(db)
{
    ui->setupUi(this);

    connect(ui->loanSelectButton, &QPushButton::clicked, this, [this]() {
        CatalogueItem* item = on_loanSelectButton_clicked_helper();
        if (item) {
            emit itemSelected(item);
        }
    });

    connect(ui->holdsSelectButton, &QPushButton::clicked, this, [this]() {
        CatalogueItem* item = on_holdsSelectButton_clicked_helper();
        if (item) {
            emit itemSelected(item);
        }
    });


    // Pointer starts null; previous code inverted condition and executed when patron WAS set.
    if (!patron) {
        ui->AIWNameLabel->setText("Patron not found");
        ui->loansTableWidget->setEnabled(false);
        ui->loanSelectButton->setEnabled(false);
        ui->holdsTableWidget->setEnabled(false);
        ui->holdsSelectButton->setEnabled(false);
    }

}

AccountDetailsPage::~AccountDetailsPage()
{
    for (auto item : activeLoans) delete item;
    activeLoans.clear();
    for (auto& row : activeHolds) delete row.item;
    activeHolds.clear();
    delete ui;
}

QPushButton* AccountDetailsPage::loanSelectButton() const {
    return ui->loanSelectButton;
}

QPushButton* AccountDetailsPage::holdsSelectButton() const {
    return ui->holdsSelectButton;
}

void AccountDetailsPage::load_username()
{
    if (patron) {
        QString userInfo = QString("Name: %1 | Email: %2 | Card #: %3")
                               .arg(QString::fromStdString(patron->get_username()))
                               .arg(QString::fromStdString(patron->get_email()))
                               .arg(patron->get_card_number());
        ui->AIWNameLabel->setText(userInfo);
    }
}

void AccountDetailsPage::load_user_loans()
{
    ui->loansTableWidget->clearContents();

    if (!patron) {
    // Removed non-patron loans debug
        ui->loansTableWidget->setEnabled(false);
        ui->loanSelectButton->setEnabled(false);
        ui->holdsTableWidget->setEnabled(false);
        ui->holdsSelectButton->setEnabled(false);
        QMessageBox::critical(this, "User Error", "Logged in user is not a Patron. Cannot load loans.");
        return;
    }

    // Use new DB API: get active loans by username, then map to items
    // Clear old loans
    for (auto item : activeLoans) delete item;
    activeLoans.clear();
    auto loans = dbManager->getActiveLoans(patron->get_username());

    if (loans.empty()) {
        ui->loansTableWidget->setRowCount(1);
        QTableWidgetItem *item = new QTableWidgetItem("No active loans.");
        ui->loansTableWidget->setItem(0, 0, item);
        ui->loansTableWidget->setSpan(0, 0, 1, 3);
        ui->loansTableWidget->setEnabled(false);
        ui->loanSelectButton->setEnabled(false);
        return;
    }

    ui->loansTableWidget->setRowCount(static_cast<int>(loans.size()));
    ui->loansTableWidget->clearSpans();
    for (int row = 0; row < static_cast<int>(loans.size()); ++row) {
        Loan* loan = loans[row];
        int isbn = loan->getIsbn();
        QString dueDateStr = QString::fromStdString(loan->getDueDate());

        CatalogueItem* item = dbManager->getItemByIsbn(isbn);
        if (!item) {
            // fallback placeholder if item missing
            ui->loansTableWidget->setItem(row, 0, new QTableWidgetItem(QString("Item %1").arg(isbn)));
        } else {
            activeLoans.push_back(item);
            ui->loansTableWidget->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(item->get_title())));
        }

        QDate dueDate = QDate::fromString(dueDateStr, "yyyy-MM-dd");
        QDate currentDate = QDate::currentDate();
        qint64 daysLeft = currentDate.daysTo(dueDate);

        ui->loansTableWidget->setItem(row, 1, new QTableWidgetItem(dueDate.toString("yyyy-MM-dd")));
        ui->loansTableWidget->setItem(row, 2, new QTableWidgetItem(QString::number(daysLeft)));
    }

    // Clean up loan objects
    for (auto loan : loans) {
        delete loan;
    }

    ui->loansTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

void AccountDetailsPage::load_user_holds(){
    ui->holdsTableWidget->clearContents();

    if (!patron) {
    // Removed non-patron holds debug
        ui->loansTableWidget->setEnabled(false);
        ui->loanSelectButton->setEnabled(false);
        ui->holdsTableWidget->setEnabled(false);
        ui->holdsSelectButton->setEnabled(false);
        QMessageBox::critical(this, "User Error", "Logged in user is not a Patron. Cannot load holds.");
        return;
    }

    for (auto& row : activeHolds) delete row.item;
    activeHolds.clear();


   // find hold items
    std::vector<CatalogueItem*> items = dbManager->getPatronHolds(patron);
    for(CatalogueItem* item : items){
        int pos = dbManager->getHoldPosition(item, patron);
        if (pos >= 0){
            activeHolds.push_back({item, pos, dbManager->readyForPickup(item, patron)});
        }
    }

    if (activeHolds.empty()) {
            ui->holdsTableWidget->setRowCount(1);
            auto* item = new QTableWidgetItem("No active holds.");
            ui->holdsTableWidget->setItem(0, 0, item);
            ui->holdsTableWidget->setSpan(0, 0, 1, 3);
            ui->holdsTableWidget->setEnabled(false);
            ui->holdsSelectButton->setEnabled(false);
            return;
    }

    ui->holdsTableWidget->setRowCount(static_cast<int>(activeHolds.size()));
    ui->holdsTableWidget->clearSpans();
    for(int i = 0; i < static_cast<int>(activeHolds.size()); i++){
        const auto& hold = activeHolds[i];

        // get loan data
        auto title = QString::fromStdString(hold.item->get_title());
        auto pos = QString::number(hold.queuePosition + 1);
        auto note = QString::fromStdString(hold.readyForPickup ? "Ready for pickup" : "In Queue");

        // create table items
        auto* titleItem = new QTableWidgetItem(title);
        auto* posItem = new QTableWidgetItem(pos);
        auto* noteItem = new QTableWidgetItem(note);

        // push to table
        ui->holdsTableWidget->setItem(i, 0, titleItem);
        ui->holdsTableWidget->setItem(i, 1, posItem);
        ui->holdsTableWidget->setItem(i, 2, noteItem);
    }

    ui->holdsTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

CatalogueItem* AccountDetailsPage::on_loanSelectButton_clicked_helper(){
    if (!ui->loansTableWidget->isEnabled()) {
        QMessageBox::information(this, "No Loans",
                                 "There are no active loans to select.");
        return nullptr;
    }

    int selectedRow = ui->loansTableWidget->currentRow();
    if (selectedRow == -1) {
        QMessageBox::information(this, "No Selection",
                                 "Please select an item from the loans list first.");
        return nullptr;
    }

    if (!patron) {
            QMessageBox::critical(this, "User Error",
                                  "Logged in user is not a Patron.");
            return nullptr;
        }

    if (selectedRow >= static_cast<int>(activeLoans.size())) {
    // Removed out of bounds loans debug
        return nullptr;
    }

    return activeLoans[selectedRow];
}

CatalogueItem* AccountDetailsPage::on_holdsSelectButton_clicked_helper(){
    if (!ui->holdsTableWidget->isEnabled()) {
        QMessageBox::information(this, "No Holds",
                                 "There are no active holds to select.");
        return nullptr;
    }

    int selectedRow = ui->holdsTableWidget->currentRow();
    if (selectedRow == -1) {
        QMessageBox::information(this, "No Selection",
                                 "Please select an item from the holds list first.");
        return nullptr;
    }

    // get catalogue item from activeHolds
    if(selectedRow >= static_cast<int>(activeHolds.size())) {
    // Removed out of bounds holds debug
        return nullptr;
    }

    return activeHolds[selectedRow].item;
}

void AccountDetailsPage::refresh()
{
    qDebug() << "AccountDetailsPage::refresh() start";
    if (!patron) {
        ui->AIWNameLabel->setText("No User Found");
        ui->loansTableWidget->setEnabled(false);
        ui->loanSelectButton->setEnabled(false);
        ui->holdsTableWidget->setEnabled(false);
        ui->holdsSelectButton->setEnabled(false);
        return;
    }
      qDebug() << "AccountDetailsPage::refresh() end";

    // Enable widgets and reload data
    ui->loansTableWidget->setEnabled(true);
    ui->loanSelectButton->setEnabled(true);
    ui->holdsTableWidget->setEnabled(true);
    ui->holdsSelectButton->setEnabled(true);
    load_username();
    load_user_loans();
    load_user_holds();
}
