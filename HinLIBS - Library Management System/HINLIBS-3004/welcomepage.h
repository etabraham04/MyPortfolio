#ifndef WELCOMEPAGE_H
#define WELCOMEPAGE_H

#include <QWidget>
#include <QLabel>
#include <user.h>
#include "ui_welcomepage.h"

class WelcomePage : public QWidget
{
    Q_OBJECT

public:
    WelcomePage(QWidget *parent = nullptr);
    ~WelcomePage();
    void updateWelcomeMessage(User* user);

private:
    Ui::WelcomePage *ui;
};

#endif // WELCOMEPAGE_H
