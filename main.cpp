#include "mainwindow.hpp"
#include <QApplication>
#include <QDesktopWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle("fusion");
    app.setApplicationName("TeQtSQLiteManager");
    app.setApplicationVersion("1.0");
    app.setObjectName("TernyukCorporation");
    app.setOrganizationDomain("www.igorternyuk.com");
    MainWindow window;
    //window.move(QApplication::desktop()->rect().center() - window.rect().center());
    window.show();

    return app.exec();
}
