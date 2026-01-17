#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <memory>
#include <iostream>
#include <QLabel>
#include <QTextEdit>
#include <QVBoxLayout>

#include <admin.h>
#include <patron.h>
#include <librarian.h>
#include <fiction.h>
#include <nonFiction.h>
#include <movie.h>
#include <magazine.h>
#include <videoGame.h>
#include <itemcontrol.h>


#include <qdebug.h>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), loggedUser(nullptr)
{
    qDebug() << "MainWindow constructor start";
        ui->setupUi(this);
        qDebug() << "UI setup complete";


    // Initialize DBManager
    dbManager = DBManager();


    navBar = new NavBar(this);
    navBar->hide(); // Hide initially until user logs in

    loginPage = new LoginPage(this);
    welcomePage = new WelcomePage(this);
    accountDetailsPage = new AccountDetailsPage(this, &dbManager);
    viewItemPage = new ViewItem(this, &dbManager);
    browseCataloguePage = new BrowseCatalogue(this, &dbManager);
    browsePatronsPage = new browsePatrons(this, &dbManager);
    createItemPage = new createitempage(this, &dbManager);
    //browsePatronPage = new browsepatronpage (this, &dbManager);

    // Create a stacked widget for pages (without navbar)
    pagesWidget = new QStackedWidget(this);
    pagesWidget->addWidget(loginPage);
    pagesWidget->addWidget(welcomePage);
    pagesWidget->addWidget(accountDetailsPage);
    pagesWidget->addWidget(browseCataloguePage);
    pagesWidget->addWidget(viewItemPage);
    pagesWidget->addWidget(browsePatronsPage);
    pagesWidget->addWidget(createItemPage);


    // Create main container with navbar on top and pages below
    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(navBar);      // Navbar at top - persists across pages
    mainLayout->addWidget(pagesWidget); // Pages below - changes based on navigation

    // Set the main widget as central widget
    this->setCentralWidget(mainWidget);

    // Start by showing login page (normal behavior, no auto-login test override)
    pagesWidget->setCurrentWidget(loginPage);

    // ============= SIGNAL CONNECTIONS =============

    connect(createItemPage->getCreateButton(), &QPushButton::clicked,
              this, &MainWindow::on_createItemButtonClicked);

    // Login button clicked
    connect(loginPage->loginButton(), &QPushButton::clicked, this, [this]()
            {
        QString usernameInput = loginPage->findChild<QTextEdit*>("usernameInput")->toPlainText();

        if(loggedUser){
            delete loggedUser;
            loggedUser = nullptr;
        }

        loggedUser = dbManager.getUser(usernameInput.toStdString());
        if (loggedUser){
            // Login success debug removed
            // Configure navbar for this user type
            showNavBarForUser(loggedUser);
            // Show welcome message
            welcomePage->updateWelcomeMessage(loggedUser);
            // Display welcome page
            pagesWidget->setCurrentWidget(welcomePage);
            // Show the navbar
            navBar->show();
            // Set patron pointer only if user is a Patron; avoids undefined 'patron' use for librarians.
            if (auto patronUser = dynamic_cast<Patron*>(loggedUser)) {
                accountDetailsPage->setPatron(patronUser);
            } else {
                accountDetailsPage->setPatron(nullptr);
            }
        } else {
            findChild<QLabel*>("incorrectCredsNotice")->show();
        } });


    // Account details page buttons
    connect(accountDetailsPage->loanSelectButton(),
            &QPushButton::clicked,
            this,
            &MainWindow::on_loanSelectButton_clicked);
    connect(accountDetailsPage->holdsSelectButton(),
            &QPushButton::clicked,
            this,
            &MainWindow::on_holdsSelectButton_clicked);

    // Navbar button signals
    connect(navBar, &NavBar::logoutClicked, this, [this]()
            {
        qDebug() << "Logout clicked";
        //LOGOUT USER TODO
    navBar->hide();
    pagesWidget->setCurrentWidget(loginPage);
    loginPage->refresh();
    // Clear patron context on logout to prevent stale access
    accountDetailsPage->setPatron(nullptr);
    accountDetailsPage->refresh(); });

    connect(navBar, &NavBar::browseCatalogueClicked, this, [this]()
            {
        qDebug() << "Browse Catalogue clicked";
        browseCataloguePage->refresh(loggedUser);
        pagesWidget->setCurrentWidget(browseCataloguePage); });

    connect(navBar, &NavBar::viewAccountClicked, this, &MainWindow::on_viewAccountClicked);

    connect(browseCataloguePage, &BrowseCatalogue::viewItemRequested,
            this, [this](CatalogueItem* item) {
        // Removed verbose item opening log
        viewItemPage->setItem(item);
        viewItemPage->setUser(loggedUser); // ensure librarian detection before item set
        pagesWidget->setCurrentWidget(viewItemPage);
    });

    connect(navBar, &NavBar::createItemClicked, this, [this](){
        qDebug() << "Create Item Page clicked";
        createItemPage->refresh();
        pagesWidget->setCurrentWidget(createItemPage);
    });

    connect(navBar, &NavBar::browsePatronsClicked, this, [this](){
        qDebug() << "Browse Patrons Page clicked";
        browsePatronsPage->refresh();
        pagesWidget->setCurrentWidget(browsePatronsPage);
    });

    connect(browsePatronsPage, &browsePatrons::viewAccountRequested,
            this, [this](Patron* user) {
        accountDetailsPage->setPatron(user);
        pagesWidget->setCurrentWidget(accountDetailsPage);
    });


}

MainWindow::~MainWindow()
{
    if(loggedUser){
        delete loggedUser;
        loggedUser = nullptr;
    }
    delete ui;
}

void MainWindow::on_loanSelectButton_clicked()
{
    CatalogueItem *item = accountDetailsPage->on_loanSelectButton_clicked_helper();
    if (!item)
        return;

    qDebug() << "MainWindow got selected loan item:"
             << QString::fromStdString(item->get_title());

    viewItemPage->setUser(loggedUser);
    viewItemPage->setItem(item);
    pagesWidget->setCurrentWidget(viewItemPage);
}

void MainWindow::on_holdsSelectButton_clicked()
{
    CatalogueItem *item = accountDetailsPage->on_holdsSelectButton_clicked_helper();
    if (!item)
        return;

    qDebug() << "MainWindow got selected hold item:"
             << QString::fromStdString(item->get_title());

    viewItemPage->setUser(loggedUser);
    viewItemPage->setItem(item);
    pagesWidget->setCurrentWidget(viewItemPage);
}

void MainWindow::showNavBarForUser(User *user)
{
    if (!user || !navBar)
        return;

    QString userType = QString::fromStdString(user->getUserType());
    QString username = QString::fromStdString(user->get_username());
    navBar->setUserType(userType, username);
    navBar->show();
}

void MainWindow::on_viewAccountClicked()
{
    qDebug() << "View Account clicked";
    if (!pagesWidget || !accountDetailsPage)
        return;
    // Ensure AccountDetailsPage knows the current patron (or nullptr if not a patron)
    Patron* patronUser = nullptr;
    if (loggedUser) {
        patronUser = dynamic_cast<Patron*>(loggedUser);
    }
    accountDetailsPage->setPatron(patronUser);
    // Ensure latest data is loaded based on the current patron context
    accountDetailsPage->refresh();
    pagesWidget->setCurrentWidget(accountDetailsPage);
}

//create ite button functionaly

void MainWindow::on_createItemButtonClicked()
{
    qDebug() << "Create item button clicked";

    QString itemType = createItemPage->getItemTypeComboBox()->currentData().toString();
    if (itemType.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please select an item type.");
        return;
    }

    // Get common fields
    QString title = createItemPage->getTitle();
    QString author = createItemPage->getAuthor();
    int year = createItemPage->getYear();
    QString isbnStr = createItemPage->getISBN();
    QString format = createItemPage->getFormat();
    QString condition = createItemPage->getCondition();

    // Validate required fields
    if (title.isEmpty() || author.isEmpty()) {
        QMessageBox::warning(this, "Error", "Title and Author are required.");
        return;
    }

    if (isbnStr.isEmpty()) {
        QMessageBox::warning(this, "Error", "ISBN is required.");
        return;
    }

    bool isbnOk;
    long isbn = isbnStr.toLong(&isbnOk);
    if (!isbnOk) {
        QMessageBox::warning(this, "Error", "ISBN must be a valid number.");
        return;
    }

    CatalogueItem* newItem = nullptr;

    try {
        if (itemType == "fiction") {
            newItem = new Fiction(
                title.toStdString(),
                author.toStdString(),
                year,
                isbn,
                format.toStdString(),
                condition.toStdString(),
                "Available"
            );

        } else if (itemType == "nonfiction") {
            int deweyDecimal = createItemPage->getDynamicField1().toInt();
            newItem = new NonFiction(
                title.toStdString(),
                author.toStdString(),
                year,
                isbn,
                format.toStdString(),
                condition.toStdString(),
                "Available",
                deweyDecimal
            );

        } else if (itemType == "magazine") {
            int issueNumber = createItemPage->getDynamicField1().toInt();
            QString publicationDate = createItemPage->getDynamicField2();

            if (publicationDate.isEmpty()) {
                QMessageBox::warning(this, "Error", "Publication Date is required for magazines.");
                return;
            }

            newItem = new Magazine(
                title.toStdString(),
                author.toStdString(),
                year,
                isbn,
                format.toStdString(),
                condition.toStdString(),
                "Available",
                issueNumber,
                publicationDate.toStdString()
            );

        } else if (itemType == "movie") {
            QString genre = createItemPage->getDynamicField1();
            QString rating = createItemPage->getDynamicField2();

            if (genre.isEmpty()) {
                QMessageBox::warning(this, "Error", "Genre is required for movies.");
                return;
            }

            newItem = new Movie(
                title.toStdString(),
                author.toStdString(),
                year,
                isbn,
                format.toStdString(),
                condition.toStdString(),
                "Available",
                genre.toStdString(),
                rating.toStdString()
            );

        } else if (itemType == "videogame") {
            QString genre = createItemPage->getDynamicField1();
            QString rating = createItemPage->getDynamicField2();

            if (genre.isEmpty()) {
                QMessageBox::warning(this, "Error", "Genre is required for video games.");
                return;
            }

            newItem = new VideoGame(
                title.toStdString(),
                author.toStdString(),
                year,
                isbn,
                format.toStdString(),
                condition.toStdString(),
                "Available",
                genre.toStdString(),
                rating.toStdString()
            );
        }

        if (newItem && dbManager.addItem(newItem)) {
            QMessageBox::information(this, "Success",
                QString("Item '%1' created successfully!").arg(title));
            createItemPage->refresh();
        } else {
            QMessageBox::critical(this, "Error", "Failed to add item to database.");
            delete newItem;
        }

    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
            QString("Failed to create item: %1").arg(e.what()));
        if (newItem) delete newItem;
    }
}

