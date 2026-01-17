#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
        qDebug() << "1. Application created";

        MainWindow w;
        qDebug() << "2. MainWindow constructed";

        w.show();
        qDebug() << "3. MainWindow shown - entering event loop";

        int result = a.exec();

        return result;
}
