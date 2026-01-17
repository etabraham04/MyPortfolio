#include "createitempage.h"
#include "ui_createitempage.h"

#include <fiction.h>
#include <nonFiction.h>
#include <movie.h>
#include <magazine.h>
#include <videoGame.h>
#include <QMessageBox>
#include <qdebug.h>

createitempage::createitempage(QWidget *parent, DBManager* db)
    : QWidget(parent)
    , ui(new Ui::createitempage)
    , dbManager(db)
    , dynamicContainer1(nullptr)
    , dynamicLayout1(nullptr)
    , dynamicContainer2(nullptr)
    , dynamicLayout2(nullptr)
    , dynamicLabel1(nullptr)
    , dynamicLabel2(nullptr)
    , dynamicInput1Line(nullptr)
    , dynamicInput1Spin(nullptr)
    , dynamicInput1Combo(nullptr)
    , dynamicInput2Line(nullptr)
    , dynamicInput2Combo(nullptr)
{
    ui->setupUi(this);

    // Hide all sections initially - only item type dropdown visible
    ui->commonFieldsContainer->hide();
    ui->dynamicContainer1->hide();
    ui->dynamicContainer2->hide();
    ui->createItemButton->hide();

    // Get references to dynamic containers
    dynamicContainer1 = ui->dynamicContainer1;
    dynamicContainer2 = ui->dynamicContainer2;

    // Set up layouts for dynamic containers
    if (dynamicContainer1) {
        dynamicLayout1 = new QVBoxLayout(dynamicContainer1);
        dynamicLayout1->setContentsMargins(0, 0, 0, 0);
    }
    if (dynamicContainer2) {
        dynamicLayout2 = new QVBoxLayout(dynamicContainer2);
        dynamicLayout2->setContentsMargins(0, 0, 0, 0);
    }

    setupItemTypes();

    // Connect the dropdown to update all fields when selection changes
    connect(ui->comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &createitempage::onItemTypeChanged);
}

createitempage::~createitempage()
{
    delete ui;
}

QPushButton* createitempage::getCreateButton() const
{
    return ui->createItemButton;
}

QComboBox* createitempage::getItemTypeComboBox() const
{
    return ui->comboBox;
}

void createitempage::setupItemTypes()
{
    ui->comboBox->clear();
    ui->comboBox->addItem("Select Item Type", "");
    ui->comboBox->addItem("Fiction Book", "fiction");
    ui->comboBox->addItem("Non-Fiction Book", "nonfiction");
    ui->comboBox->addItem("Magazine", "magazine");
    ui->comboBox->addItem("Movie", "movie");
    ui->comboBox->addItem("Video Game", "videogame");
}

void createitempage::onItemTypeChanged(int index)
{
    // If no valid selection, hide everything
    if (index <= 0) {
        ui->commonFieldsContainer->hide();
        ui->createItemButton->hide();
        clearDynamicFields();
        return;
    }

    // Show common fields for any valid item type
    ui->commonFieldsContainer->show();
    ui->createItemButton->show();

    // Create type-specific dynamic fields
    QString itemType = ui->comboBox->currentData().toString();
    createDynamicFields(itemType);
}

void createitempage::clearDynamicFields()
{
    // Hide containers
    if (dynamicContainer1) dynamicContainer1->setVisible(false);
    if (dynamicContainer2) dynamicContainer2->setVisible(false);

    // Delete existing dynamic widgets
    if (dynamicLabel1) { delete dynamicLabel1; dynamicLabel1 = nullptr; }
    if (dynamicLabel2) { delete dynamicLabel2; dynamicLabel2 = nullptr; }
    if (dynamicInput1Line) { delete dynamicInput1Line; dynamicInput1Line = nullptr; }
    if (dynamicInput1Spin) { delete dynamicInput1Spin; dynamicInput1Spin = nullptr; }
    if (dynamicInput1Combo) { delete dynamicInput1Combo; dynamicInput1Combo = nullptr; }
    if (dynamicInput2Line) { delete dynamicInput2Line; dynamicInput2Line = nullptr; }
    if (dynamicInput2Combo) { delete dynamicInput2Combo; dynamicInput2Combo = nullptr; }
}

void createitempage::createDynamicFields(const QString& itemType)
{
    clearDynamicFields();

    if (!dynamicContainer1 || !dynamicLayout1) return;

    if (itemType == "fiction") {
        // Fiction books don't need extra fields beyond common ones
        // Keep containers hidden

    } else if (itemType == "nonfiction") {
        // Non-fiction: Dewey Decimal Classification
        dynamicContainer1->setVisible(true);
        dynamicLabel1 = new QLabel("Dewey Decimal:");
        dynamicInput1Spin = new QSpinBox();
        dynamicInput1Spin->setRange(0, 999);
        dynamicInput1Spin->setValue(0);
        dynamicLayout1->addWidget(dynamicLabel1);
        dynamicLayout1->addWidget(dynamicInput1Spin);

    } else if (itemType == "magazine") {
        // Magazine: Issue Number + Publication Date
        dynamicContainer1->setVisible(true);
        dynamicLabel1 = new QLabel("Issue Number:");
        dynamicInput1Spin = new QSpinBox();
        dynamicInput1Spin->setRange(1, 10000);
        dynamicInput1Spin->setValue(1);
        dynamicLayout1->addWidget(dynamicLabel1);
        dynamicLayout1->addWidget(dynamicInput1Spin);

        if (dynamicContainer2 && dynamicLayout2) {
            dynamicContainer2->setVisible(true);
            dynamicLabel2 = new QLabel("Publication Date:");
            dynamicInput2Line = new QLineEdit();
            dynamicInput2Line->setPlaceholderText("e.g., January 2023");
            dynamicLayout2->addWidget(dynamicLabel2);
            dynamicLayout2->addWidget(dynamicInput2Line);
        }

    } else if (itemType == "movie") {
        // Movie: Genre + Rating
        dynamicContainer1->setVisible(true);
        dynamicLabel1 = new QLabel("Genre:");
        dynamicInput1Line = new QLineEdit();
        dynamicInput1Line->setPlaceholderText("e.g., Sci-Fi, Action, Drama");
        dynamicLayout1->addWidget(dynamicLabel1);
        dynamicLayout1->addWidget(dynamicInput1Line);

        if (dynamicContainer2 && dynamicLayout2) {
            dynamicContainer2->setVisible(true);
            dynamicLabel2 = new QLabel("Rating:");
            dynamicInput2Combo = new QComboBox();
            dynamicInput2Combo->addItems({"G", "PG", "PG-13", "R", "NC-17"});
            dynamicLayout2->addWidget(dynamicLabel2);
            dynamicLayout2->addWidget(dynamicInput2Combo);
        }

    } else if (itemType == "videogame") {
        // Video Game: Genre + Rating
        dynamicContainer1->setVisible(true);
        dynamicLabel1 = new QLabel("Genre:");
        dynamicInput1Line = new QLineEdit();
        dynamicInput1Line->setPlaceholderText("e.g., Action-Adventure, RPG, Platformer");
        dynamicLayout1->addWidget(dynamicLabel1);
        dynamicLayout1->addWidget(dynamicInput1Line);

        if (dynamicContainer2 && dynamicLayout2) {
            dynamicContainer2->setVisible(true);
            dynamicLabel2 = new QLabel("ESRB Rating:");
            dynamicInput2Combo = new QComboBox();
            dynamicInput2Combo->addItems({"E", "E10+", "T", "M", "AO"});
            dynamicLayout2->addWidget(dynamicLabel2);
            dynamicLayout2->addWidget(dynamicInput2Combo);
        }
    }
}

void createitempage::refresh()
{
    // Reset to initial state
    ui->comboBox->setCurrentIndex(0);
    ui->titleInput->clear();
    ui->authorInput->clear();
    ui->yearInput->setValue(2024);
    ui->isbnInput->clear();
    ui->formatInput->setCurrentIndex(0);
    ui->conditionInput->setCurrentIndex(0);

    // Hide all sections
    ui->commonFieldsContainer->hide();
    ui->createItemButton->hide();
    clearDynamicFields();
}

QString createitempage::getTitle() const
{
    return ui->titleInput->text();
}

QString createitempage::getAuthor() const
{
    return ui->authorInput->text();
}

int createitempage::getYear() const
{
    return ui->yearInput->value();
}

QString createitempage::getFormat() const
{
    return ui->formatInput->currentText();
}

QString createitempage::getCondition() const
{
    return ui->conditionInput->currentText();
}

QString createitempage::getISBN() const
{
    return ui->isbnInput->text();
}

QString createitempage::getDynamicField1() const
{
    if (dynamicInput1Line) return dynamicInput1Line->text();
    if (dynamicInput1Spin) return QString::number(dynamicInput1Spin->value());
    if (dynamicInput1Combo) return dynamicInput1Combo->currentText();
    return "";
}

QString createitempage::getDynamicField2() const
{
    if (dynamicInput2Line) return dynamicInput2Line->text();
    if (dynamicInput2Combo) return dynamicInput2Combo->currentText();
    return "";
}
