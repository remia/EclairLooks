#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QDesktopWidget>
#include <QFile>

#include "mainwindow.h"
#include "imagepipeline.h"


int main(int argc, char **argv)
{
    QCoreApplication::setApplicationName("Eclair Looks");
    QCoreApplication::setOrganizationName("Ymagis");
    QCoreApplication::setOrganizationDomain("ymagis.com");

    QSurfaceFormat format;
    format.setSamples(16);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(1);
    format.setVersion(3, 2);
    QSurfaceFormat::setDefaultFormat(format);

    ImagePipeline p;

    QApplication app(argc, argv);

    QFile cssFile(":/css/application.css");
    cssFile.open(QFile::ReadOnly);
    QString cssString = QLatin1String(cssFile.readAll());
    app.setStyleSheet(cssString);

    MainWindow mainWindow(&p);
    mainWindow.show();

    // Move window to the center of the screen
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - mainWindow.width()) / 2;
    int y = (screenGeometry.height() - mainWindow.height()) / 2;
    mainWindow.move(x, y);

    // Load default image
    p.SetInput(Image::FromFile("/Users/remi/ownCloud/Images/stresstest/LUT_Stress_Test_HD_20161224.tif"));

    return app.exec();
}
