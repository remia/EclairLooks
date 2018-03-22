#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QDesktopWidget>

#include "mainwindow.h"

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName("Ymagis");
    QCoreApplication::setOrganizationDomain("ymagis.com");

    QSurfaceFormat format;
    format.setSamples(16);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(1);
    format.setVersion(3, 2);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - mainWindow.width()) / 2;
    int y = (screenGeometry.height() - mainWindow.height()) / 2;
    mainWindow.move(x, y);

    return app.exec();
}
