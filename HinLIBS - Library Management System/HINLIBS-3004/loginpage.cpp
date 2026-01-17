#include "loginpage.h"
#include "ui_loginpage.h"
#include <QPalette>
#include <QColor>
#include <QLineEdit>
#include <qdebug.h>
LoginPage::LoginPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginPage)
{
    ui->setupUi(this);
    // Ensure login page background displays
    setAutoFillBackground(true);
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#384b06"));
    setPalette(palette);
    findChild<QLabel*>("incorrectCredsNotice")->hide();
    ui->loginButton->setStyleSheet(
        "QPushButton { background-color: #384b06; border-radius: 5px; color: #face23; }"
        "QPushButton::hover { background-color: #273601; }"
    );
}

LoginPage::~LoginPage()
{
    delete ui;
}

void LoginPage::refresh() {
    findChild<QTextEdit*>("usernameInput")->clear();
    findChild<QLabel*>("incorrectCredsNotice")->hide();
}

QPushButton* LoginPage::loginButton() const {
    return ui->loginButton;
}
