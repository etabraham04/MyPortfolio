QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    accountdetailspage.cpp \
    browseCatalogue.cpp \
    browsepatrons.cpp \
    createitempage.cpp \
    dbmanager.cpp \
    loginpage.cpp \
    main.cpp \
    mainwindow.cpp \
    navbar.cpp \
    viewItem.cpp \
    welcomepage.cpp

HEADERS += \
    accountdetailspage.h \
    admin.h \
    book.h \
    browseCatalogue.h \
    browsepatrons.h \
    catalogueItem.h \
    createitempage.h \
    dbmanager.h \
    fiction.h \
    librarian.h \
    loan.h \
    loginpage.h \
    magazine.h \
    mainwindow.h \
    movie.h \
    navbar.h \
    nonFiction.h \
    patron.h \
    user.h \
    videoGame.h \
    viewItem.h \
    welcomepage.h
FORMS += \
    accountdetailspage.ui \
    browseCatalogue.ui \
    browsepatrons.ui \
    createitempage.ui \
    loginpage.ui \
    mainwindow.ui \
    navbar.ui \
    viewItem.ui \
    welcomepage.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Copy database to build directory
copy_db.target = copy_db
copy_db.commands = $(COPY) $$PWD/hinlibs.db $$OUT_PWD
QMAKE_EXTRA_TARGETS += copy_db
POST_TARGETDEPS += copy_db
