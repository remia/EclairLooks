#include "mainwindow.h"
#include "imagewidget.h"
#include "logwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle("Eclair Look");
    setCentralWidget(new ImageWidget(this));

    QDockWidget *logDockWidget = new QDockWidget("Log");
    logDockWidget->setWidget(new LogWidget());
    addDockWidget(Qt::BottomDockWidgetArea, logDockWidget);
}