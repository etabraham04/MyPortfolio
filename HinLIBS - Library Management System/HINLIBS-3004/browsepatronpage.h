#ifndef BROWSEPATRONPAGE_H
#define BROWSEPATRONPAGE_H

#include <QWidget>
#include <dbmanager.h>

namespace Ui {
class browsepatronpage;
}

class browsepatronpage : public QWidget
{
    Q_OBJECT

public:
    explicit browsepatronpage(QWidget *parent = nullptr, DBManager* db = nullptr);
    ~browsepatronpage();

private:
    Ui::browsepatronpage *ui;
    DBManager* dbManager;
};

#endif // BROWSEPATRONPAGE_H
