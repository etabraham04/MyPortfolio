#include "welcomepage.h"
#include <QPalette>
#include <QColor>
#include <qdebug.h>
#include <admin.h>
#include <patron.h>
#include <librarian.h>
#include <user.h>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent), ui(new Ui::WelcomePage)
{
    ui->setupUi(this);
    
    // Ensure background fills completely
    setAutoFillBackground(true);
    
    // Set stylesheet for the widget - this will override parent backgrounds
    setStyleSheet(
        "WelcomePage { "
        "  background-color: #384b06; "
        "  border: none; "
        "  margin: 0px; "
        "  padding: 0px; "
        "} "
        "WelcomePage > QVBoxLayout { "
        "  margin: 0px; "
        "  padding: 0px; "
        "}"
    );
    
    // Also set palette as a backup
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#384b06"));
    setPalette(palette);
}

WelcomePage::~WelcomePage()
{
    delete ui;
}

void WelcomePage::updateWelcomeMessage(User* user)
{
    if (!user) return;

    std::string username = user->get_username();
    std::string userType = user->getUserType();

    // Welcome page updated debug removed
}
