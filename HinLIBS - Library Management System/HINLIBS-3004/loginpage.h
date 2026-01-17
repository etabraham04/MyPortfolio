#ifndef LOGINPAGE_H
#define LOGINPAGE_H

#include <QWidget>
#include <QPushButton>

namespace Ui {
class LoginPage;
}

class LoginPage : public QWidget
{
    Q_OBJECT

public:
    explicit LoginPage(QWidget *parent = nullptr);
    ~LoginPage();
    QPushButton* loginButton() const;
    void refresh();

private:
    Ui::LoginPage *ui;
};

#endif // LOGINPAGE_H
