#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include <loginpage.h>
#include <accountdetailspage.h>
#include <browseCatalogue.h>
#include <welcomepage.h>
#include <navbar.h>
#include "viewItem.h"
#include <dbmanager.h>
#include <browsepatrons.h>
#include "createitempage.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void showBrowseCatalogue();

private:
    Ui::MainWindow *ui;
    DBManager dbManager;
    User* loggedUser;
    QStackedWidget *pagesWidget; // holds app pages below the navbar

    LoginPage* loginPage;
    BrowseCatalogue* browseCataloguePage;
    ViewItem* viewItemPage;
    WelcomePage* welcomePage;
    AccountDetailsPage* accountDetailsPage;
    NavBar *navBar;
    browsePatrons* browsePatronsPage;
    createitempage* createItemPage;


    // member functions
    void showNavBarForUser(User* user);  // show navbar for specific user
    void on_loanSelectButton_clicked();
    void on_holdsSelectButton_clicked();
    void on_viewAccountClicked();
    void on_createItemButtonClicked();
};
#endif // MAINWINDOW_H
