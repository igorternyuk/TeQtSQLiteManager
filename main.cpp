#include "mainwindow.hpp"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle("fusion");
    MainWindow w;
    w.move(QApplication::desktop()->rect().center() - w.rect().center());
    w.show();

    return a.exec();
}
