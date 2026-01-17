#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDebug>
#include <QSpacerItem>
#include <QSizePolicy>
#include <QColor>
#include <QPainter>
#include "navbar.h"

// Role colors matched to provided swatches
namespace {
    constexpr const char* kNavColorLibrarian = "#62c3c3"; // teal/cyan
    constexpr const char* kNavColorAdmin     = "#e19a2d"; // orange
    constexpr const char* kNavColorPatron    = "#b07bb0"; // purple
}

NavBar::NavBar(QWidget *parent)
    : QWidget(parent), ui(nullptr)
{
    // Set size policy to expand horizontally
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setMinimumHeight(50);
    setMaximumHeight(50);
    
    //paint own background, avoid palette interference
    setAutoFillBackground(false);
    
    setupUI();
}

NavBar::~NavBar()
{
    // ui is created in setupUI and will be deleted automatically
    // as children of this widget
}

void NavBar::setupUI()
{
    // Create main layout
    mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Create user type label (left side)
    userTypeLabel = new QLabel("", this);
    userTypeLabel->setStyleSheet("QLabel { "
                                  "color: white; "
                                  "font-weight: bold; "
                                  "font-size: 16px; "
                                  "background-color: transparent; "
                                  "padding-left: 20px; "
                                  "padding-right: 20px; "
                                  "}");
    userTypeLabel->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    mainLayout->addWidget(userTypeLabel);

    // Add stretch to push buttons to the right
    mainLayout->addStretch();

    // Create buttons with separator lines
    browseCatalogueButton = new QPushButton("Browse\nCatalogue", this);
    viewAccountButton = new QPushButton("View\nAccount", this);
    logoutButton = new QPushButton("Logout", this);
    browsePatronsButton = new QPushButton("Browse\nPatrons", this);
    createItemButton = new QPushButton("Create\nItem", this);

    // Style buttons with transparent background and white text, with borders as separators
    QString buttonStyle = "QPushButton { "
                          "background-color: transparent; "
                          "color: white; "
                          "font-weight: bold; "
                          "border: none; "
                          "border-left: 2px solid white; "
                          "padding: 5px 25px; "
                          "font-size: 13px; "
                          "} "
                          "QPushButton:hover { background-color: rgba(255, 255, 255, 0.1); }";
    
    browseCatalogueButton->setStyleSheet(buttonStyle);
    viewAccountButton->setStyleSheet(buttonStyle);
    logoutButton->setStyleSheet(buttonStyle);
    browsePatronsButton->setStyleSheet(buttonStyle);
    createItemButton->setStyleSheet(buttonStyle);

    // Set button heights
    browseCatalogueButton->setMinimumHeight(50);
    viewAccountButton->setMinimumHeight(50);
    logoutButton->setMinimumHeight(50);
    browsePatronsButton->setMinimumHeight(50);
    createItemButton->setMinimumHeight(50);
    
    browseCatalogueButton->setMinimumWidth(140);
    viewAccountButton->setMinimumWidth(140);
    logoutButton->setMinimumWidth(100);
    browsePatronsButton->setMinimumWidth(140);
    createItemButton->setMinimumWidth(140);

    // Connect button signals
    connect(logoutButton, &QPushButton::clicked, this, &NavBar::on_logoutButton_clicked);
    connect(browseCatalogueButton, &QPushButton::clicked, this, &NavBar::on_browseCatalogueButton_clicked);
    connect(viewAccountButton, &QPushButton::clicked, this, &NavBar::on_viewAccountButton_clicked);
    connect(browsePatronsButton, &QPushButton::clicked, this, [this](){ emit browsePatronsClicked(); });
    connect(createItemButton, &QPushButton::clicked, this, [this](){ emit createItemClicked(); });

    // Set default background (patron purple)
    applyStyle(kNavColorPatron);

    // Initially hide all buttons
    browseCatalogueButton->hide();
    viewAccountButton->hide();
    logoutButton->hide();
    browsePatronsButton->hide();
    createItemButton->hide();
    
    // Order adjusted: Create Item, Browse Catalogue, Browse Patrons, View Account (if shown), Logout
    mainLayout->addWidget(createItemButton);
    mainLayout->addWidget(browseCatalogueButton);
    mainLayout->addWidget(browsePatronsButton);
    mainLayout->addWidget(viewAccountButton);
    mainLayout->addWidget(logoutButton);
}

void NavBar::setUserType(const QString& userType, const QString& username)
{
    // Hide all buttons first
    browseCatalogueButton->hide();
    viewAccountButton->hide();
    logoutButton->hide();
    browsePatronsButton->hide();
    createItemButton->hide();

    if (userType.toLower() == "patron") {
        userTypeLabel->setText(username + ": patron");
        applyStyle(kNavColorPatron); // Purple for patron

        // Show patron buttons
        browseCatalogueButton->show();
        viewAccountButton->show();
        logoutButton->show();
    } else if (userType.toLower() == "librarian") {
        userTypeLabel->setText(username + ": librarian");
        applyStyle(kNavColorLibrarian); // Cyan for librarian

        // Show librarian buttons
        browseCatalogueButton->show();
        browsePatronsButton->show();
        createItemButton->show();
        logoutButton->show();
    } else if (userType.toLower() == "admin") {
        userTypeLabel->setText(username + ": admin");
        applyStyle(kNavColorAdmin); // Orange for admin

        // Show only logout button
        logoutButton->show();
    } else {
        userTypeLabel->setText("");
        applyStyle(kNavColorPatron); // Default purple
    }
}

void NavBar::applyStyle(const QString& backgroundColor)
{
    // Store and repaint; actual fill happens in paintEvent
    backgroundColor_ = backgroundColor;
    update();
}

QPushButton* NavBar::getLogoutButton() const
{
    return logoutButton;
}

QPushButton* NavBar::getBrowseCatalogueButton() const
{
    return browseCatalogueButton;
}

QPushButton* NavBar::getViewAccountButton() const
{
    return viewAccountButton;
}

void NavBar::on_logoutButton_clicked()
{
    emit logoutClicked();
}

void NavBar::on_browseCatalogueButton_clicked()
{
    emit browseCatalogueClicked();
}

void NavBar::on_viewAccountButton_clicked()
{
    emit viewAccountClicked();
}

void NavBar::on_createItemButton_clicked()
{
    emit createItemClicked();
}

void NavBar::on_browsePatronsButton_clicked()
{
    emit browsePatronsClicked();
}

void NavBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.fillRect(rect(), QColor(backgroundColor_));
    QWidget::paintEvent(event);
}
