#include <locale>

#include <QtWidgets/QApplication>
#include <QtGui/QSurfaceFormat>
#include <QtWidgets/QDesktopWidget>
#include <QFile>

#include <core/imagepipeline.h>
#include <parameter/parameterseriallist.h>
#include <gui/mainwindow.h>
#include "operator/imageoperatorlist.h"
#include "operator/ocio/matrix.h"
#include "operator/ocio/filetransform.h"
#include "operator/ocio/colorspace.h"
#include "operator/ctl/operator.h"


int main(int argc, char **argv)
{
    QCoreApplication::setApplicationName("Eclair Looks");
    QCoreApplication::setOrganizationName("Ymagis");
    QCoreApplication::setOrganizationDomain("ymagis.com");
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts);
    QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);

    QSurfaceFormat format;
    format.setSamples(16);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::TripleBuffer);
    format.setSwapInterval(1);
    format.setVersion(3, 2);
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    // Settings
    using FP = FilePathParameter;
    ParameterSerialList settings;
    settings.Add<FP>("Default Image", "", "Choose an image", "");
    settings.Add<FP>("Image Base Folder", "", "Choose a folder", "", FP::PathType::Folder);
    settings.Add<FP>("Look Base Folder", "", "Choose a folder", "", FP::PathType::Folder);
    settings.Add<FP>("Look Tonemap LUT", "", "Choose a LUT", "");

    // Pipeline & Operators
    ImagePipeline pipeline;
    pipeline.SetName("main");

    // First try to load image from application settings
    // If this fails, use default embeded image
    std::string imgPath = settings.Get<FilePathParameter>("Default Image")->value();
    Image img = Image::FromFile(imgPath);

    if (!img) {
        QFile f = QFile(":/images/stresstest.exr");
        if (f.open(QIODevice::ReadOnly)) {
            QByteArray blob = f.readAll();
            img = Image::FromBuffer((void *) blob.data(), blob.count());
        }
    }

    if (img)
        pipeline.SetInput(img);

    ImageOperatorList operators;
    operators.Register<OCIOMatrix>();
    operators.Register<OCIOFileTransform>();
    operators.Register<OCIOColorSpace>();
    operators.Register<CTLTransform>();

    QApplication app(argc, argv);

    // Make sure we use C locale to parse float
    // This need to be placed right after QApplication initialization
    // See : http://doc.qt.io/qt-5/qcoreapplication.html#locale-settings
    QLocale::setDefault(QLocale("C"));

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
    mainWindow.centerOnScreen();

    pipeline.Init();

    return app.exec();
}
