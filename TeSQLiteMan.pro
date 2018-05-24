#-------------------------------------------------
#
# Project created by QtCreator 2018-05-19T16:09:11
#
#-------------------------------------------------

QT       += core gui sql printsupport
CONFIG += c++1z
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TeSQLiteMan
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tetreewidget.cpp \
    settings_util.cpp \
    findreplacedialog.cpp \
    preferencesdialog.cpp \
    addnewtabledialog.cpp

HEADERS  += mainwindow.hpp \
    tetreewidget.hpp \
    settings_util.hpp \
    preferencesdialog.hpp \
    findreplacedialog.hpp \
    defines.h \
    addnewtabledialog.hpp

FORMS    += mainwindow.ui \
    findreplacedialog.ui \
    preferencesdialog.ui \
    addnewtabledialog.ui

RESOURCES += \
    res.qrc
