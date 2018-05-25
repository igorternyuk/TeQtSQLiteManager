#include "defines.h"
#include "mainwindow.hpp"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setStyle(APP_STYLE);
    app.setApplicationName(APP_NAME);
    app.setApplicationVersion(APP_VERSION);
    app.setObjectName(CORP_NAME);
    app.setOrganizationDomain(APP_DOMAIN);
    MainWindow window;
    window.show();
    return app.exec();
}
