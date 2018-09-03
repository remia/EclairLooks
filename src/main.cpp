#include <locale>

#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QDesktopWidget>
#include <QFile>

#include "mainwindow.h"
#include "imagepipeline.h"
#include "settings.h"
#include "operator/imageoperatorlist.h"
#include "operator/ociomatrix_operator.h"
#include "operator/ociofiletransform_operator.h"
#include "operator/ociocolorspace_operator.h"
#include "operator/ctl_operator.h"


int main(int argc, char **argv)
{
    QCoreApplication::setApplicationName("Eclair Looks");
    QCoreApplication::setOrganizationName("Ymagis");
    QCoreApplication::setOrganizationDomain("ymagis.com");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);

    QSurfaceFormat format;
    format.setSamples(16);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(1);
    format.setVersion(3, 2);
    QSurfaceFormat::setDefaultFormat(format);

    // Settings
    Settings settings;

    // Pipeline & Operators
    ImagePipeline pipeline;
    std::string imgPath = settings.GetParameter<FilePathParameter>("Default Image").value;
    pipeline.SetInput(Image::FromFile(imgPath));

    ImageOperatorList operators;
    operators.Register<OCIOMatrix>();
    operators.Register<OCIOFileTransform>();
    operators.Register<OCIOColorSpace>();
    operators.Register<CTLTransform>();

    QApplication app(argc, argv);

    // Make sure we use C locale to parse float
    // This need to be placed right after QApplication initialization
    // See : http://doc.qt.io/qt-5/qcoreapplication.html#locale-settings
    setlocale(LC_NUMERIC,"C");

    QFile cssFile(":/css/application.css");
    cssFile.open(QFile::ReadOnly);
    QString cssString = QLatin1String(cssFile.readAll());
    app.setStyleSheet(cssString);

    MainWindow mainWindow;
    mainWindow.setPipeline(&pipeline);
    mainWindow.setOperators(&operators);
    mainWindow.setSettings(&settings);
    mainWindow.setup();
    mainWindow.show();

    // Move window to the center of the screen
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - mainWindow.width()) / 2;
    int y = (screenGeometry.height() - mainWindow.height()) / 2;
    mainWindow.move(x, y);

    pipeline.Init();

    // NOTE : Waiting for a OpenImageIO release that includes the new IOProxy feature
    // QFile f = QFile(":/images/stresstest.png");
    // if (f.open(QIODevice::ReadOnly)) {
    //     QByteArray blob = f.readAll();
    //     p.SetInput(Image::FromBuffer((void *) blob.data(), blob.count()));
    // }

    return app.exec();
}
