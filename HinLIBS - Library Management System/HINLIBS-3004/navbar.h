#ifndef NAVBAR_H
#define NAVBAR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QDebug>

namespace Ui {
class NavBar;
}

class NavBar : public QWidget
{
    Q_OBJECT

public:
    explicit NavBar(QWidget *parent = nullptr);
    ~NavBar();

    // Set the navbar based on user type
    void setUserType(const QString& userType, const QString& username);

    // Getter methods for buttons
    QPushButton* getLogoutButton() const;
    QPushButton* getBrowseCatalogueButton() const;
    QPushButton* getViewAccountButton() const;
    QPushButton* getBrowsePatronsButton() const { return browsePatronsButton; }
    QPushButton* getCreateItemButton() const { return createItemButton; }

signals:
    void logoutClicked();
    void browseCatalogueClicked();
    void viewAccountClicked();
    void browsePatronsClicked();
    void createItemClicked();

private slots:
    void on_logoutButton_clicked();
    void on_browseCatalogueButton_clicked();
    void on_viewAccountButton_clicked();
    void on_createItemButton_clicked();
    void on_browsePatronsButton_clicked();

private:
    Ui::NavBar *ui;
    
    // UI elements
    QLabel *userTypeLabel;
    QPushButton *logoutButton;
    QPushButton *browseCatalogueButton;
    QPushButton *viewAccountButton;
    QPushButton *browsePatronsButton {nullptr};
    QPushButton *createItemButton {nullptr};
    QHBoxLayout *mainLayout;

    void setupUI();
    void applyStyle(const QString& backgroundColor);

    // Store current background color for custom painting
    QString backgroundColor_ {"#c97ec9"};

protected:
    void paintEvent(QPaintEvent *event) override;
};

#endif // NAVBAR_H
