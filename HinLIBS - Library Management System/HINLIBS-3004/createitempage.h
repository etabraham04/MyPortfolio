#ifndef CREATEITEMPAGE_H
#define CREATEITEMPAGE_H

#include <QWidget>
#include <QPushButton>
#include <QComboBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <dbmanager.h>

namespace Ui {
class createitempage;
}

class createitempage : public QWidget
{
    Q_OBJECT

public:
    explicit createitempage(QWidget *parent = nullptr, DBManager* db = nullptr);
    ~createitempage();

        QPushButton* getCreateButton() const;
        QComboBox* getItemTypeComboBox() const;


        void refresh();

        QString getTitle() const;
        QString getAuthor() const;
        int getYear() const;
        QString getFormat() const;
        QString getCondition() const;
        QString getISBN() const;
        QString getDynamicField1() const;
        QString getDynamicField2() const;


private slots:
    void onItemTypeChanged(int index);


private:
    Ui::createitempage *ui;
    DBManager* dbManager;


    QWidget* dynamicContainer1;
    QVBoxLayout* dynamicLayout1;
    QWidget* dynamicContainer2;
    QVBoxLayout* dynamicLayout2;

    QLabel* dynamicLabel1;
    QLabel* dynamicLabel2;
    QLineEdit* dynamicInput1Line;
    QSpinBox* dynamicInput1Spin;
    QComboBox* dynamicInput1Combo;
    QLineEdit* dynamicInput2Line;
    QComboBox* dynamicInput2Combo;

    void setupItemTypes();
    void clearDynamicFields();
    void createDynamicFields(const QString& itemType);


};

#endif // CREATEITEMPAGE_H
