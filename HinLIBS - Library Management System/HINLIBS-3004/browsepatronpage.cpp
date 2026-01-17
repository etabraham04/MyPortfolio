#include "browsepatronpage.h"
#include "ui_browsepatronpage.h"

browsepatronpage::browsepatronpage(QWidget *parent, DBManager* db) :
    QWidget(parent),
    ui(new Ui::browsepatronpage),
    dbManager(db)
{
    ui->setupUi(this);
}

browsepatronpage::~browsepatronpage()
{
    delete ui;
}
